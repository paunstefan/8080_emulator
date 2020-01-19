#include <SDL2/SDL.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include "Display.hpp"

const int WIDTH = 224;
const int HEIGHT = 256;
const int DISPLAY_WIDTH = 896;
const int DISPLAY_HEIGHT = 1024;

Display::Display(){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL could not initialize! Error: %s\n", SDL_GetError());
		exit(1);
	}

	this->window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_WIDTH, DISPLAY_HEIGHT,
									SDL_WINDOW_SHOWN);
	if(window == NULL){
		printf("SDL could not create window! Error: %s\n", SDL_GetError());
		exit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

	sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
								SDL_TEXTUREACCESS_STREAMING,
								WIDTH, HEIGHT);
}

/**
	Sets a certain pixel to be white or black.
	@param x: column
	@param y: row
*/
void Display::set_pixel(uint32_t x, uint32_t y, uint8_t pixel){
	uint8_t *p = (uint8_t*)this->pixels + (HEIGHT - x - 1) * (224*sizeof(int32_t)) + y * 4;

	*(uint32_t*)p = pixel ? 0xFFFFFF : 0;
}

/**
	Loops over the memory mapped video RAM to translate it to the SDL2 screen.
	@param arr: Space Invaders screen memory map
*/
void Display::update_surface(uint8_t *arr){
	int x = 0;
	int y = 0;
	for(int i = 0; i < WIDTH*HEIGHT/8; i++){
		uint8_t b = arr[i];
		//for(int j = 7; j >= 0; j--){
			for(int j = 0; j <= 7; j++){
			uint8_t pixel = (b >> j) & 1;
			set_pixel(x, y, pixel);

			if(((x + 1) % 256) == 0){
				x = 0;
				y++;
			}
			else{
				x++;
			}
		}
	}
}

/**
	Updates the screen.
	@param arr: Space Invaders screen memory map
*/
void Display::show_frame(uint8_t *arr){
	this->update_surface(arr);
	SDL_UpdateTexture(sdlTexture, NULL, pixels, 224 * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
}
