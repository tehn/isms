#include <SDL2/SDL.h>
#include <stdio.h>

#include "events.h"
#include "metro.h"


/* 
Copyright (c) 2021 tehn@nnnnnnnn.co

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE.
*/

#define HOR 32
#define VER 16
#define PAD 2

int WIDTH = 8 * HOR + 8 * PAD * 2;
int HEIGHT = 8 * (VER + 2) + 8 * PAD * 2;
int FPS = 30, GUIDES = 1, BIGPIXEL = 0, ZOOM = 2;

SDL_Window *gWindow;
SDL_Renderer *gRenderer;
SDL_Texture *gTexture;
uint32_t *pixels;


void
clear(uint32_t *dst)
{
	int v, h;
	for(v = 0; v < HEIGHT; v++)
		for(h = 0; h < WIDTH; h++)
			dst[v * WIDTH + h] = 0;
}

void
putpixel(uint32_t *dst, int x, int y, int color)
{
	if(x >= 0 && x < WIDTH - 8 && y >= 0 && y < HEIGHT - 8)
		dst[(y + PAD * 8) * WIDTH + (x + PAD * 8)] = color;
}

void
redraw(uint32_t *dst)
{
	clear(dst);
  putpixel(dst,20,20,0xFFFFFF);
  putpixel(dst,30,30,0xAAAAAA);
	SDL_UpdateTexture(gTexture, NULL, dst, WIDTH * sizeof(uint32_t));
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
	SDL_RenderPresent(gRenderer);
}


int
error(char *msg, const char *err)
{
	printf("Error %s: %s\n", msg, err);
	return 0;
}

void
quit(void)
{
	free(pixels);
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
	exit(0);
}


void
domouse(SDL_Event *event)
{
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

void
dokey(SDL_Event *event)
{
  /*
	int shift = SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT;
  */
	int ctrl = SDL_GetModState() & KMOD_LCTRL || SDL_GetModState() & KMOD_RCTRL;
	if(ctrl) {
		switch(event->key.keysym.sym) {
		/* Generic */
		case SDLK_n: printf("hello!\n"); break;
		}
	} else {
		switch(event->key.keysym.sym) {
		case SDLK_1: break;
		case SDLK_ESCAPE: break;
		}
	}
}

int
init(void)
{
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
	return 1;
}

int
main(int argc, char **argv)
{
	int ticknext = 0;

	if(!init())
		return error("Init", "Failure");
	if(argc > 1) {
    /*
		if(!opendoc(&doc, argv[1]))
			makedoc(&doc, argv[1]);
      */
	}

  events_init();
  metros_init();

	while(1) {
		int tick = SDL_GetTicks();
		SDL_Event event;
		if(tick < ticknext)
			SDL_Delay(ticknext - tick);
		ticknext = tick + (1000 / FPS);
		while(SDL_PollEvent(&event) != 0) {
			switch(event.type) {
			case SDL_QUIT: quit(); break;
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEMOTION: domouse(&event); break;
			case SDL_KEYDOWN: dokey(&event); break;
			case SDL_WINDOWEVENT:
				if(event.window.event == SDL_WINDOWEVENT_EXPOSED)
					redraw(pixels);
				break;
			}
		}
	}
	quit();
	return 0;
}
