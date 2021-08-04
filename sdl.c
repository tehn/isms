#include <SDL2/SDL.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "events.h"
#include "sdl.h"

SDL_Window *gWindow;
SDL_Renderer *gRenderer;
SDL_Texture *gTexture;
uint32_t *pixels;

int WIDTH = 8 * HOR + 8 * PAD * 2;
int HEIGHT = 8 * (VER + 2) + 8 * PAD * 2;
int FPS = 30, GUIDES = 1, BIGPIXEL = 0, ZOOM = 2;


void *sdl_loop(void *);
static pthread_t p;

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
	if(x >= 0 && x < WIDTH - 8 && y >= 0 && y < HEIGHT - 8)
		dst[(y + PAD * 8) * WIDTH + (x + PAD * 8)] = color;
}

void redraw(uint32_t *dst) {
	clear(dst);
  putpixel(dst,20,20,0xFFFFFF);
  putpixel(dst,30,30,0xAAAAAA);
	SDL_UpdateTexture(gTexture, NULL, dst, WIDTH * sizeof(uint32_t));
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
	SDL_RenderPresent(gRenderer);
}


void domouse(SDL_Event *event) {
  /*
	int ctrl = SDL_GetModState() & KMOD_LCTRL || SDL_GetModState() & KMOD_RCTRL;
  */

	switch(event->type) {
	case SDL_MOUSEBUTTONUP:
		if(event->button.button == SDL_BUTTON_LEFT)
		if(event->button.button == SDL_BUTTON_RIGHT)
		break;
	case SDL_MOUSEBUTTONDOWN:
		if(event->motion.y / ZOOM / 8 - PAD == VER + 1) {
			return;
		}
		/*if(event->button.button == SDL_BUTTON_LEFT)
		if(event->button.button == SDL_BUTTON_RIGHT)
		if(event->button.button == SDL_BUTTON_MIDDLE)
    */
		redraw(pixels);
		break;
	case SDL_MOUSEMOTION:
    /*
		brush.x = screenpos(event->motion.x, brush.vx);
		brush.y = screenpos(event->motion.y, brush.vy);
    */
		break;
	}
}

void dokey(SDL_Event *event) {
  /*
	int shift = SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT;
  */
	int ctrl = SDL_GetModState() & KMOD_LCTRL || SDL_GetModState() & KMOD_RCTRL;
	if(ctrl) {
		switch(event->key.keysym.sym) {
		case SDLK_n: printf("hello!\n"); break;
		}
	} else {
		switch(event->key.keysym.sym) {
		case SDLK_1: printf("one\n"); break;
		case SDLK_ESCAPE: break;
		}
	}
}

int init_sdl(void) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		return error("Init", SDL_GetError());
	gWindow = SDL_CreateWindow("niil",
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

  if (pthread_create(&p, NULL, sdl_loop, 0)) {
    return error("SDL", "pthread failed");
  }

	return 1;
}

void deinit_sdl(void) {
  printf("deinit_sdl\n");
	free(pixels);
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
  printf("end of sdl\n");
  pthread_cancel(p);
}


void *sdl_loop(void *x) {
  (void)x;

	SDL_Event event;
  while(1) {
    //int tick = SDL_GetTicks();
		//if(tick < ticknext)
			//SDL_Delay(ticknext - tick);
		//ticknext = tick + (1000 / FPS);
		while(SDL_PollEvent(&event) != 0) {
			switch(event.type) {
			//case SDL_MOUSEBUTTONUP:
			//case SDL_MOUSEBUTTONDOWN:
			//case SDL_MOUSEMOTION: //domouse(&event); break;
			case SDL_KEYDOWN: dokey(&event); break;
			case SDL_QUIT:
        printf("sdl event\n");
        union event_data *ev = event_data_new(EVENT_QUIT);
        event_post(ev);
        break;
			//case SDL_WINDOWEVENT:
				//if(event.window.event == SDL_WINDOWEVENT_EXPOSED)
					//redraw(pixels);
			}
		}
    sleep(0.001);
  }
}
