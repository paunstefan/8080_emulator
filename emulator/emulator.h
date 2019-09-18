#include <stdint.h>

#pragma once

struct condition_codes{
	uint8_t z:1;	// zero (1 if result == 0)
	uint8_t s:1;	// sign (1 if 7th bit is set)
	uint8_t p:1;	// parity (1 when result is even)
	uint8_t cy:1;	// carry (1 when result carried)
	uint8_t ac:1;	// auxillary carry - not implemented
	uint8_t pad:3;
};

/**
	CPU state structure. Contains all registers, the memory array and a check if it allows interrupts.
*/
typedef struct state_8080{
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	uint16_t sp;
	uint16_t pc;
	uint8_t *memory;
	struct condition_codes cc;
	uint8_t int_enable;
} state_8080;


/**
	Prints an error message and exits the program when the emulator hits an unimplemented instruction.
	@param state: the CPU state
*/
void unimplemented_instruction(state_8080 *state);

/**
	Executes an Intel 8080 instruction
	@param state: the CPU state
	@return the number of cycles the instruction takes
*/
uint8_t emulate_8080_op(state_8080 *state);

/**
	Emulates a system interrupt.
	@param state: the CPU state
	@param interrupt_number: the interrupt number (only 1 and 2 used)
*/
void generate_interrupt(state_8080 *state, uint32_t interrupt_number);

/**
	Calculates the parity of an integer (number of 1s in the binary representation).
	@param x: the number
	@param size: bit width
	@return 1 if even number of 1s, 0 if odd number of 1s
*/
uint32_t parity(uint32_t x, uint32_t size);

/**
	Wraps the memory writing operation so it blocks attempted writes to the ROM.
	@param state: the CPU state
	@param addr: RAM address
	@param val: value to write
*/
void write_ram(state_8080 *state, uint16_t addr, uint8_t val);

/**
	Push a value to the stack.
	@param state: the CPU state
	@param high: the MSB
	@param low: the LSB
*/
void push(state_8080 *state, uint8_t high, uint8_t low);

/**
	Pops a value from the stack
	@param state: the CPU state
	@param high: the MSB destination
	@param low: the LSB destination
*/
void pop(state_8080 *state, uint8_t *high, uint8_t *low);