/*
 * metro.c
 *
 * accurate metros using pthreads and clock_nanosleep.
 */

// std
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// posix / linux
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>

// norns
#include "event.h"
#include "metro.h"
#include "lua.h"

#define MAX_NUM_METROS_OK 36

enum { METRO_STATUS_RUNNING, METRO_STATUS_STOPPED };

const int MAX_NUM_METROS = MAX_NUM_METROS_OK;
struct metro {
    int idx;                     // metro index
    int status;                  // running/stopped status
    double seconds;              // period in seconds
    uint64_t count;              // total iterations ( <=0 -> infinite )
    uint64_t stage;              // current count of iterations
    uint64_t time;               // current time (in nsec)
    uint64_t delta;              // current delta (in nsec)
    pthread_t tid;               // thread id
    pthread_mutex_t stage_lock;  // mutex protecting stage number
    pthread_mutex_t status_lock; // mutex protecting status
};

struct metro metros[MAX_NUM_METROS_OK];

//---------------------------
//---- static declarations

static void metro_handle_error(int code, const char *msg) {
    fprintf(stderr, "error code: %d (%s) in \"%s\"\n", code, strerror(code), msg);
}

static void metro_init(struct metro *t, uint64_t nsec, int count);
static void metro_set_current_time(struct metro *t);
static void *metro_thread_loop(void *metro);
static void metro_bang(struct metro *t);
static void metro_sleep(struct metro *t);
static void metro_reset(struct metro *t, int stage);
static void metro_cancel(struct metro *t);

// lua functions

static int _start(lua_State *l);
static int _stop(lua_State *l);

//------------------------
//---- extern definitions

void init_metro(void) {
  for (int i = 0; i < MAX_NUM_METROS_OK; i++) {
    metros[i].status = METRO_STATUS_STOPPED;
    metros[i].seconds = 1.0;
  }

  // lua
  lua_newtable(L);
  lua_reg_func("start",_start);
  lua_reg_func("stop",_stop);
  lua_setglobal(L,"metro");
}

void deinit_metro() {
  for(int i=0;i<MAX_NUM_METROS_OK;i++)
    metro_stop(i);
}


void metro_start(int idx, double seconds, int count, int stage) {
    uint64_t nsec;

    if ((idx >= 0) && (idx < MAX_NUM_METROS_OK)) {
        struct metro *t = &metros[idx];
        pthread_mutex_lock(&t->status_lock);
        if (t->status == METRO_STATUS_RUNNING) {
            metro_cancel(t);
        }
        pthread_mutex_unlock(&t->status_lock);
        if (seconds > 0.0) {
            metros[idx].seconds = seconds;
        }
        nsec = (uint64_t)(metros[idx].seconds * 1000000000.0);
        metros[idx].idx = idx;
        metro_reset(&metros[idx], stage);
        metro_init(&metros[idx], nsec, count);
    } else {
        fprintf(stderr, "invalid metro index, not added. max count of metros is %d\n", MAX_NUM_METROS_OK);
    }
}

void metro_stop(int idx) {
    if ((idx >= 0) && (idx < MAX_NUM_METROS_OK)) {
        pthread_mutex_lock(&(metros[idx].status_lock));
        if (metros[idx].status == METRO_STATUS_STOPPED) {
            // fprintf(stderr, "metro_stop: already stopped\n");
            ;
            ; // nothing to do
        } else {
            metro_cancel(&metros[idx]);
        }
        pthread_mutex_unlock(&(metros[idx].status_lock));
    } else {
        fprintf(stderr, "metro_stop(): invalid metro index, max count of metros is %d\n", MAX_NUM_METROS_OK);
    }
}

void metro_set_time(int idx, float sec) {
    // fprintf(stderr, "metro_set_time(%d, %f)\n", idx, sec);
    if ((idx >= 0) && (idx < MAX_NUM_METROS_OK)) {
        metros[idx].seconds = sec;
        metros[idx].delta = (uint64_t)(sec * 1000000000.0);
    }
}

//------------------------
//---- static definitions

static void metro_reset(struct metro *t, int stage) {
    pthread_mutex_lock(&(t->stage_lock));
    if (stage > 0) {
        t->stage = stage;
    } else {
        t->stage = 0;
    }
    pthread_mutex_unlock(&(t->stage_lock));
}

void metro_init(struct metro *t, uint64_t nsec, int count) {
    int res;
    pthread_attr_t attr;

    res = pthread_attr_init(&attr);
    if (res != 0) {
        metro_handle_error(res, "pthread_attr_init");
        return;
    }

    // set other thread attributes here...
    res = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (res != 0) {
        metro_handle_error(res, "pthread_attr_init");
        return;
    }
    res |= pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (res != 0) {
        metro_handle_error(res, "pthread_attr_init");
        return;
    }

    t->delta = nsec;
    t->count = count;
    res = pthread_create(&(t->tid), &attr, &metro_thread_loop, (void *)t);
    if (res != 0) {
        metro_handle_error(res, "pthread_create");
        return;
    } else {
        t->status = METRO_STATUS_RUNNING;
        if (res != 0) {
            metro_handle_error(res, "pthread_setschedparam");
            switch (res) {
            case ESRCH:
                fprintf(stderr, "specified thread does not exist\n");
                assert(false);
                break;
            case EINVAL:
                fprintf(stderr, "invalid thread policy value or associated parameter\n");
                assert(false);
                break;
            case EPERM:
                fprintf(stderr, "failed to set scheduling priority.\n");
                // this doesn't need to assert; it can happen with wrong
                // permissions
                // still good for user to know about
                break;
            default:
                fprintf(stderr, "unknown error code\n");
                assert(false);
            } /* switch */
            return;
        }
    }
}

void *metro_thread_loop(void *metro) {
    struct metro *t = (struct metro *)metro;
    int stop = 0;

    pthread_mutex_lock(&(t->status_lock));
    t->status = METRO_STATUS_RUNNING;
    pthread_mutex_unlock(&(t->status_lock));

    metro_set_current_time(t);
    while (!stop) {
        metro_sleep(t);
        pthread_mutex_lock(&(t->stage_lock));
        if ((t->stage >= t->count) && (t->count > 0)) {
            stop = 1;
        }
        pthread_mutex_unlock(&(t->stage_lock));

        if (stop) {
            break;
        }
        pthread_testcancel();

        pthread_mutex_lock(&(t->stage_lock));
        metro_bang(t);
        t->stage += 1;
        pthread_mutex_unlock(&(t->stage_lock));
    }
    pthread_mutex_lock(&(t->status_lock));
    t->status = METRO_STATUS_STOPPED;
    pthread_mutex_unlock(&(t->status_lock));
    return NULL;
}

void metro_set_current_time(struct metro *t) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    t->time = (uint64_t)((1000000000 * (int64_t)time.tv_sec) + (int64_t)time.tv_nsec);
}

void metro_bang(struct metro *t) {
    union event_data *ev = event_data_new(EVENT_METRO);
    ev->metro.id = t->idx;
    ev->metro.stage = t->stage;
    event_post(ev);
}

void metro_sleep(struct metro *t) {
    struct timespec ts;
    t->time += t->delta;
    ts.tv_sec = t->time / 1000000000;
    ts.tv_nsec = t->time % 1000000000;
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
}

void metro_wait(int idx) {
    pthread_join(metros[idx].tid, NULL);
}

void metro_cancel(struct metro *t) {
    // NB: no, we don't want to lock the state mutex here,
    // b/c we're already locking in callers
    if (t->status == METRO_STATUS_STOPPED) {
        fprintf(stderr, "metro_cancel(): already stopped. shouldn't get here\n");
        return;
    }
    int ret = pthread_cancel(t->tid);
    if (ret) {
        fprintf(stderr, "metro_stop(): pthread_cancel() failed; error: ");
        switch (ret) {
        case ESRCH:
            fprintf(stderr, "specified thread does not exist\n");
            break;
        default:
            fprintf(stderr, "unknown error code\n");
            assert(false);
        }
    } else {
        t->status = METRO_STATUS_STOPPED;
    }
}

// lua functions

static int _start(lua_State *l) {
  //printf("metro start\n");
  lua_check_num_args(4);
  double idx = luaL_checknumber(l, 1);
  double seconds = luaL_checknumber(l, 2);
  double count = luaL_checknumber(l, 3);
  double stage = luaL_checknumber(l, 4);
  metro_start(idx, seconds, count, stage);
  lua_settop(l, 0);
  return 0;
}

static int _stop(lua_State *l) {
  //printf("metro stop\n");
  lua_check_num_args(1);
  double idx = luaL_checknumber(l, 1);
  metro_stop(idx);
  lua_settop(l, 0);
  return 0;
}



// lua event

void event_metro(int idx, int stage) {
  //printf("e: metro: %i %i\n",idx, stage);
  lua_getglobal(L, "metro");
  lua_getfield(L, -1, "tick");
  lua_remove(L, -2);
  lua_pushinteger(L, idx + 1);   // convert to 1-based
  lua_pushinteger(L, stage + 1); // convert to 1-based
  l_report(L, l_docall(L, 2, 0));
  //if (!(lua_pcall(L, 2, 0, 0) == LUA_OK)) {
  //printf("Error on run method\n");
  //}
}

#undef MAX_NUM_METROS_OK

