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

SDL_Window *gWindow;
SDL_Renderer *gRenderer;
SDL_Texture *gTexture;
uint32_t *pixels;

int WIDTH = 256;
int HEIGHT = 128;
int ZOOM = 4;

void *sdl_loop(void *);

static int _redraw(lua_State *l);
static int _clear(lua_State *l);
static int _pixel(lua_State *l);

void sdl_check();

int error(char *msg, const char *err) {
	printf("Error %s: %s\n", msg, err);
	return 0;
}


void clear(uint32_t *dst) {
	int v, h;
	for(v = 0; v < HEIGHT; v++)
		for(h = 0; h < WIDTH; h++)
			dst[v * WIDTH + h] = 0;
}

void putpixel(uint32_t *dst, int x, int y, int color) {
	if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
		dst[y * WIDTH + x] = color;
}

void redraw(uint32_t *dst) {
	SDL_UpdateTexture(gTexture, NULL, dst, WIDTH * sizeof(uint32_t));
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
	SDL_RenderPresent(gRenderer);
}

int init_sdl(void) {
  //printf(">> SDL: init\n");

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		return error("Init", SDL_GetError());
	gWindow = SDL_CreateWindow("isms",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH * ZOOM,
		HEIGHT * ZOOM,
		SDL_WINDOW_SHOWN);
	if(gWindow == NULL)
		return error("Window", SDL_GetError());
	gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
	if(gRenderer == NULL)
		return error("Renderer", SDL_GetError());
	gTexture = SDL_CreateTexture(gRenderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STATIC,
		WIDTH,
		HEIGHT);
	if(gTexture == NULL)
		return error("Texture", SDL_GetError());
	pixels = (uint32_t *)malloc(WIDTH * HEIGHT * sizeof(uint32_t));
	if(pixels == NULL)
		return error("Pixels", "Failed to allocate memory");
	clear(pixels);

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
  lua_setglobal(L,"screen");
}

void deinit_sdl(void) {
  //printf(">> SDL: deinit\n");
  pthread_cancel(p);
	free(pixels);
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
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
            case SDLK_r: printf("reload script! (dummy)\n");
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
        //case SDL_WINDOWEVENT:
        //if(event.window.event == SDL_WINDOWEVENT_EXPOSED)
        //redraw(pixels);
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
    clear(pixels);
    lua_settop(l, 0);
    return 0;
}

int _pixel(lua_State *l) {
    lua_check_num_args(3);
    double x = luaL_checknumber(l, 1);
    double y = luaL_checknumber(l, 2);
    double c = luaL_checknumber(l, 3);
    putpixel(pixels,x,y,c);
    lua_settop(l, 0);
    return 0;
}
