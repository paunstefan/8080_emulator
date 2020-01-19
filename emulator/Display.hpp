#include <SDL2/SDL.h>
#include <cstdint>

#pragma once

/**
	Display class. Wraps the SDL2 interactions.
*/
struct Display{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *sdlTexture = NULL;

	uint32_t pixels[256*224];

public:
	Display();

	/**
		Sets a certain pixel to be white or black.
		@param x: column
		@param y: row
	*/
	void set_pixel(uint32_t x, uint32_t y, uint8_t pixel);

	/**
		Loops over the memory mapped video RAM to translate it to the SDL2 screen.
		@param arr: Space Invaders screen memory map
	*/
	void update_surface(uint8_t *arr);

	/**
		Updates the screen.
		@param arr: Space Invaders screen memory map
	*/
	void show_frame(uint8_t *arr);
};
