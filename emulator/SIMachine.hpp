#include <chrono>
#include <cstdint>
#include "Display.hpp"
#include "emulator.h"

#pragma once

/**
	Space Invaders Machine class. Emulates the arcade machine hardware.
*/
struct SIMachine{
	state_8080 *state;

	// timers for the interrupts
	double last_timer;
	double next_int;
	uint8_t which_int;

	// shift register variables
	uint8_t shift0;
	uint8_t shift1;
	uint8_t shift_offset;
	uint8_t in_port1;

	Display *display;

	/**
		Initializes the CPU, Display and reads ROM files.
	*/
	SIMachine();

	~SIMachine();

	/**
		Reads ROM file to memory.
		@param filename: ROM file name
		@param offset: location in memory to read it
	*/
	void read_2_memory(const char *filename, uint32_t offset);

	/**
		Runs the CPU and triggers the interrupts if it is the case.
	*/
	void run();

	/**
		Runs an infinite loop with the game.
	*/
	void start_emulation();

	/**
		Emulates the system input ports (input to CPU).
		@param port: port number
		@return value read
	*/
	uint8_t input_SI(uint8_t port);

	/**
		Emulates the system output ports. (from CPU to machine)
		@param port: port number
		@param value: value to output
	*/
	void output_SI(uint8_t port, uint8_t value);

	/**
		@return the location of the RAM frame buffer.
	*/
	uint8_t *get_framebuffer();
};