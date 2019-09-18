#include <cstdint>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include "SIMachine.hpp"
#include "Display.hpp"
#include "emulator.h"

/**
	Initializes the CPU, Display and reads ROM files.
*/
SIMachine::SIMachine(){
	this->state = (state_8080*)calloc(sizeof(state_8080), 1);
	this->state->memory = (uint8_t*)malloc(65535);	// allocate the whole memory map
	this->state->pc = 0;
	this->state->sp = 0xf000;
	this->last_timer = 0.0;

	this->state->int_enable = 1;
	this->state->a = 0;
	this->state->b = 0;
	this->state->c = 0;
	this->state->d = 0;
	this->state->e = 0;
	this->state->h = 0;
	this->state->l = 0;

	this->read_2_memory("invaders/invaders.h", 0x0);
	this->read_2_memory("invaders/invaders.g", 0x800);
	this->read_2_memory("invaders/invaders.f", 0x1000);
	this->read_2_memory("invaders/invaders.e", 0x1800);

	this->display = new Display();
	// for(int i = 0; i < 8192; i++){
	// 	this->rom_save[i] = this->state->memory[i];
	// }
}

SIMachine::~SIMachine(){
	free(this->state->memory);
	free(this->state);
}

/**
	Runs an infinite loop with the game.
*/
void SIMachine::start_emulation(){
	using namespace std::this_thread;
	using namespace std::chrono;

	while(1){
		this->run();
		sleep_for(milliseconds(1));
	}
}

/**
	Runs the CPU and triggers the interrupts if it is the case.
*/
void SIMachine::run(){
	using namespace std::chrono;

	// get current time from epoch in microseconds
	double now = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();

	// first execution
	if(this->last_timer == 0.0){
		this->last_timer = now;
		this->next_int = this->last_timer + 16000.0;	// 16 milliseconds from now (60FPS)
		this->which_int = 1;
	}

	if((this->state->int_enable) && (now > this->next_int)){
		// switches between interrupts
		// 1 in the middle of the frame, 2 at the end
		if(this->which_int == 1){
			generate_interrupt(this->state, 1);
			this->which_int = 2;
		}
		else{
			generate_interrupt(this->state, 2);
			this->which_int = 1;
			this->display->show_frame(this->get_framebuffer());	// update screen
		}

		this->next_int = now + 8000.0; 	// half a frame
	}

	// calculate how many cycles have passed to execute the needed number of instructions
	// this emulates the 2MHz clock of the machine
	double since_last = now - this->last_timer;
	int32_t cycles_to_execute = 2 * since_last;
	int32_t cycles = 0;

	while(cycles_to_execute > cycles){
		uint8_t *op;
		op = this->state->memory + this->state->pc;
		if(*op == 0xdb){
			// IN
			this->state->a = this->input_SI(op[1]);
			this->state->pc += 2;
			cycles += 3;
		}
		else if(*op == 0xd3){
			// OUT
			this->output_SI(op[1], this->state->a);
			this->state->pc += 2;
			cycles += 3;
		}
		else{
			cycles += emulate_8080_op(this->state);
		}
	}

	this->last_timer = now;
}

/**
	Reads ROM file to memory.
	@param filename: ROM file name
	@param offset: location in memory to read it
*/
void SIMachine::read_2_memory(const char *filename, uint32_t offset){
	FILE* f = fopen(filename, "rb");
	if(f == NULL){
		printf("ERROR: couldn't open file\n");
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	uint32_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

	fread(this->state->memory + offset, size, 1, f);
	fclose(f);
}

/**
	@return the location of the RAM frame buffer.
*/
uint8_t* SIMachine::get_framebuffer(){
	return this->state->memory + 0x2400;
}

/**
	Emulates the system input ports (input to CPU).
	@param port: port number
	@return value read
*/
uint8_t SIMachine::input_SI(uint8_t port){
	uint8_t a = 0;
	switch(port){
		case 0:
			return 1;
		case 1:
			return 0;
		case 3:
			{
				uint16_t v = (this->shift1 << 8) | this->shift0;
				a = ((v >> (8 - this->shift_offset)) & 0xff);
			}
			break;
	}
	return a;
}

/**
	Emulates the system output ports. (from CPU to machine)
	@param port: port number
	@param value: value to output
*/
void SIMachine::output_SI(uint8_t port, uint8_t value){
	switch(port){
		case 2:
			this->shift_offset = value & 0x7;
			break;
		case 4:
			this->shift0 = this->shift1;
			this->shift1 = value;
			break;
	}
}