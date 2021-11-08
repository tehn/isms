#include <SDL2/SDL.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "lua.h"

#include "event.h"
#include "sdl.h"

static pthread_t p;

SDL_Window *window;
SDL_Surface *surface;
SDL_Surface *screen;
SDL_Rect rect;

uint32_t *pixels;

int WIDTH = 256;
int HEIGHT = 128;
int ZOOM = 4;

void *sdl_loop(void *);

static int _redraw(lua_State *l);
static int _clear(lua_State *l);
static int _pixel(lua_State *l);
static int _line(lua_State *l);

void sdl_check();

int error(char *msg, const char *err) {
  printf("Error %s: %s\n", msg, err);
  return 0;
}

void rerect() {
  rect.x = 0;
  rect.y = 0;
  rect.w = WIDTH*ZOOM;
  rect.h = HEIGHT*ZOOM;
  SDL_FillRect(screen, NULL, 0);
}


void clear(uint32_t *dst) {
  int v, h;
  for(v = 0; v < HEIGHT; v++)
    for(h = 0; h < WIDTH; h++)
      dst[v * WIDTH + h] = 0;
}

void pixel(uint32_t *dst, int x, int y, int color) {
  if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
    dst[y * WIDTH + x] = color;
}

void line(uint32_t *dst, int ax, int ay, int bx, int by, int color) {
  int dx = abs(bx - ax), sx = ax < bx ? 1 : -1;
  int dy = -abs(by - ay), sy = ay < by ? 1 : -1;
  int err = dx + dy, e2;
  for(;;) {
    pixel(dst, ax, ay, color);
    if(ax == bx && ay == by)
      break;
    e2 = 2 * err;
    if(e2 >= dy) {
      err += dy;
      ax += sx;
    }
    if(e2 <= dx) {
      err += dx;
      ay += sy;
    }
  }
}

void redraw(uint32_t *dst) {
  SDL_BlitScaled(surface, NULL, screen, &rect);
  SDL_UpdateWindowSurface(window);
}

int init_sdl(void) {
  //printf(">> SDL: init\n");

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    return error("Init", SDL_GetError());
  window = SDL_CreateWindow("isms",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      WIDTH * ZOOM,
      HEIGHT * ZOOM,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if(window == NULL)
    return error("Window", SDL_GetError());
  SDL_SetWindowMinimumSize(window, WIDTH, HEIGHT);

  surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
  screen = SDL_GetWindowSurface(window);
  rerect();

  clear(surface->pixels);

  // start event check timer
  if (pthread_create(&p, NULL, sdl_loop, 0)) {
    return error("SDL", "pthread failed");
  }

  return 1;
}

void register_sdl(void) {
  lua_newtable(L);
  lua_reg_func("redraw",_redraw);
  lua_reg_func("clear",_clear);
  lua_reg_func("pixel",_pixel);
  lua_reg_func("line",_line);
  lua_setglobal(L,"screen");
}

void deinit_sdl(void) {
  //printf(">> SDL: deinit\n");
  pthread_cancel(p);
  SDL_DestroyWindow(window);
  window = NULL;
  SDL_FreeSurface(surface);
  SDL_Quit();
}

void reset_sdl(void) {
  clear(surface->pixels);
}


void sdl_check() {
  union event_data *ev;
  SDL_Event event;
  int ctrl;
  while(SDL_PollEvent(&event) != 0) {
    switch(event.type) {
      //case SDL_MOUSEBUTTONUP:
      //case SDL_MOUSEBUTTONDOWN:
      //case SDL_MOUSEMOTION: //domouse(&event); break;
      case SDL_KEYDOWN: 
        //shift = SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT;
        ctrl = SDL_GetModState() & KMOD_LCTRL || SDL_GetModState() & KMOD_RCTRL;
        if(ctrl) {
          switch(event.key.keysym.sym) {
            case SDLK_q: ev = event_data_new(EVENT_QUIT);
                         event_post(ev);
                         break;
            case SDLK_r: ev = event_data_new(EVENT_RELOAD);
                         event_post(ev);
                         break;
            case SDLK_MINUS:
                         if(ZOOM>1) {
                           ZOOM--;
                           rerect();
                           redraw(pixels);
                         }
                         break;
            case SDLK_EQUALS:
                         ZOOM++;
                         rerect();
                         redraw(pixels);
                         break;
          }
        } 
        ev = event_data_new(EVENT_KEY);
        ev->key.scancode = event.key.keysym.sym;
        event_post(ev);
        break;
      case SDL_QUIT:
        ev = event_data_new(EVENT_QUIT);
        event_post(ev);
        break;
      case SDL_WINDOWEVENT:
        //if(event.window.event == SDL_WINDOWEVENT_EXPOSED)
        //redraw(pixels);
        if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
          screen = SDL_GetWindowSurface(window);
          rerect();
          redraw(pixels);
        }
    }
  }
}

void *sdl_loop(void *x) {
  (void)x;
  union event_data *ev;
  while(1) {
    ev = event_data_new(EVENT_SDL_CHECK);
    event_post(ev);
    sleep(0.001);
  }
}


///////

int _redraw(lua_State *l) {
  lua_check_num_args(0);
  redraw(pixels);
  lua_settop(l, 0);
  return 0;
}

int _clear(lua_State *l) {
  lua_check_num_args(0);
  clear(surface->pixels);
  lua_settop(l, 0);
  return 0;
}

int _pixel(lua_State *l) {
  lua_check_num_args(3);
  double x = luaL_checknumber(l, 1);
  double y = luaL_checknumber(l, 2);
  double c = luaL_checknumber(l, 3);
  pixel(surface->pixels,x,y,c);
  lua_settop(l, 0);
  return 0;
}

int _line(lua_State *l) {
  lua_check_num_args(5);
  double x1 = luaL_checknumber(l, 1);
  double y1 = luaL_checknumber(l, 2);
  double x2 = luaL_checknumber(l, 3);
  double y2 = luaL_checknumber(l, 4);
  double c = luaL_checknumber(l, 5);
  line(surface->pixels,x1,y1,x2,y2,c);
  lua_settop(l, 0);
  return 0;
}
