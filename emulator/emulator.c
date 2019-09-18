#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "emulator.h"

// 1 if you want to show the CPU state in the terminal.
#define PRINTOP 0

uint8_t cycles8080[] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x10..0x1f
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, //etc
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,
	
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
	
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, 
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11, 
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11, 
};

// For debugging
uint32_t disassemble8080op(uint8_t *buffer, uint32_t pc);

/**
	Prints an error message and exits the program when the emulator hits an unimplemented instruction.
	@param state: the CPU state
*/
void unimplemented_instruction(state_8080 *state){
	printf("ERROR: Unimplemented instruction: %02x\n PC: %04x", state->memory[state->pc - 1], state->pc - 1);
	exit(1);
}

/**
	Calculates the parity of an integer (number of 1s in the binary representation).
	@param x: the number
	@param size: bit width
	@return 1 if even number of 1s, 0 if odd number of 1s
*/
uint32_t parity(uint32_t x, uint32_t size){
	uint32_t p = 0;
	x = (x & ((1 << size) - 1));
	for(unsigned int i = 0; i < size; i++){
		if(x & 0x1){
			p++;
		}
		x = x >> 1;
	}
	return (0 == (p & 0x1));
}

/**
	Wraps the memory writing operation so it blocks attempted writes to the ROM.
	@param state: the CPU state
	@param addr: RAM address
	@param val: value to write
*/
void write_ram(state_8080 *state, uint16_t addr, uint8_t val){
	if(addr < 0x2000){
		return;
	}
	if(addr >= 0x4000){
		return;
	}
	state->memory[addr] = val;
}

/**
	Push a value to the stack.
	@param state: the CPU state
	@param high: the MSB
	@param low: the LSB
*/
void push(state_8080 *state, uint8_t high, uint8_t low){
	write_ram(state, state->sp-1, high);
	write_ram(state, state->sp-2, low);
	state->sp -=  2;
}

/**
	Pops a value from the stack
	@param state: the CPU state
	@param high: the MSB destination
	@param low: the LSB destination
*/
void pop(state_8080 *state, uint8_t *high, uint8_t *low){
	*low = state->memory[state->sp];
	*high = state->memory[state->sp + 1];
	state->sp += 2;
}

/**
	Executes an Intel 8080 instruction
	@param state: the CPU state
	@return the number of cycles the instruction takes
*/
uint8_t emulate_8080_op(state_8080 *state){
	uint8_t *opcode = state->memory + state->pc;

	//disassemble8080op(state->memory, state->pc);

	state->pc += 1;
	switch(*opcode){
		case 0x00:
			// NOP
			break;
		case 0x01:
			// LXI B, d16
			state->c = opcode[1];
			state->b = opcode[2];
			state->pc += 2;
			break;
		case 0x02:
			// STAX B
			{
				uint16_t offset = (state->b << 8) | (state->c);
				write_ram(state, offset, state->a);
			}
			break;
		case 0x03:
			// INX B
			state->c++;
			if(state->c == 0){
				state->b++;
			}
			break;
		case 0x04:
			//INR B
			{
				uint16_t answer = state->b + 1;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				state->b = answer;
			}
			break;
		case 0x05:
			// DCR B
			{

			uint8_t answer = state->b - 1;
			state->cc.z = (answer == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->b = answer;
			}
			break;
		case 0x06:
			// MVI B, d8
			state->b = opcode[1];
			state->pc += 1;
			break;
		case 0x07:
			// RLC
			{
				uint8_t answer = state->a;
				state->a = ((answer & 0x80) >> 7) || (answer << 1);
				state->cc.cy = (0x80 == (answer & 0x80));
			}
			break;
		case 0x08:
			
			break;
		case 0x09:
			// DAD B
			{
			uint32_t bc = (state->b << 8) | (state->c);
			uint32_t hl = (state->h << 8) | (state->l);
			uint32_t answer = bc + hl;
			state->h = (answer >> 8) & 0xff;
			state->l = answer & 0xff;
			state->cc.cy = (answer > 0xffff);
			}
			break;
		case 0x0A:
			// LDAX B
			{
				uint16_t offset = (state->b << 8) | state->c;
				state->a = state->memory[offset];
			}
			break;
		case 0x0B:
			state->c -= 1;
			if(state->c == 0xff){
				state->b -=1;
			}
			break;
		case 0x0C:
			// INR C
			{
				uint16_t answer = state->c + 1;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				state->c = answer;
			}
			break;
		case 0x0D:
			// DCR C
			{
			uint8_t answer = state->c - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->c = answer;
			}
			break;
		case 0x0E:
			// MVI C, d8
			state->c = opcode[1];
			state->pc += 1;
			break;
		case 0x0F:
			// RRC
			{
			uint8_t low = state->a & 0x1;
			state->a = (low << 7) | (state->a >> 1);
			state->cc.cy = (low == 1);
			}
			break;

		case 0x10: 
			
			break;
		case 0x11:
			// LXI D, d16
			state->e = opcode[1];
			state->d = opcode[2];
			state->pc += 2;
			break;
		case 0x12:
			// STAX D
			{
				uint16_t offset = (state->d << 8) | (state->e);
				write_ram(state, offset, state->a);
			}
			break;
		case 0x13:
			// INX D
			state->e++;
			if(state->e == 0){
				state->d++;
			}
			break;
		case 0x14:
			// INR D
		{
			uint16_t answer = state->d + 1;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->d = answer;
		}
			break;
		case 0x15:
			// DCR D
		{
			uint16_t answer = state->d - 1;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->d = answer;
		}
			break;
		case 0x16:
			// MVI D, d8
			state->d = opcode[1];
			state->pc += 1;
			break;
		case 0x17:
			// RAL
		{
			uint8_t answer = state->a;
			state->a = state->cc.cy | (answer << 1);
			state->cc.cy = (0x80 == (answer&0x80));
		}
			break;
		case 0x18:

			break;
		case 0x19:
			// DAD D
			{
			uint32_t de = (state->d << 8) | (state->e);
			uint32_t hl = (state->h << 8) | (state->l);
			uint32_t answer = de + hl;
			state->h = (uint8_t)(answer >> 8) & 0xff;
			state->l = (uint8_t)answer & 0xff;
			state->cc.cy = (answer > 0xffff);
			}
			break;
		case 0x1A:
			// LDAX D
			{
			uint32_t offset = (state->d << 8) | (state->e);
			state->a = state->memory[offset];
			}
			break;
		case 0x1B:
			// DCX D
			state->e -= 1;
			if(state->e == 0xff){
				state->d -=1;
			}
			break;
		case 0x1C:
			// INR E
		{
			uint16_t answer = state->e + 1;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->e = answer;
		}
			break;
		case 0x1D:
			// DCR E
		{
			uint16_t answer = state->e - 1;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->e = answer;
		}
			break;
		case 0x1E:
			// MVI E, d8
			state->e = opcode[1];
			state->pc += 1;
			break;
		case 0x1F:
			// RAR
			{
			uint8_t answer = state->a;
			state->a = (state->cc.cy << 7) | (answer >>1);
			state->cc.cy = (1 == (answer & 1));
			}	
			break;

		case 0x20: 
			
			break;
		case 0x21:
			// LXI H, d16
			state->l = opcode[1];
			state->h = opcode[2];
			state->pc += 2;
			break;
		case 0x22:
			// SHDL d16
		{
			uint32_t offset = (opcode[2] << 8) | opcode[1];
			write_ram(state, offset, state->l);
			write_ram(state, offset + 1, state->h);
			state->pc += 2;
		}
			break;
		case 0x23:
			// INX H
			state->l++;
			if(state->l == 0){
				state->h++;
			}
			break;
		case 0x24:
			// INR H
			{
				uint16_t answer = state->h + 1;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				state->h = answer;
			}
			break;
		case 0x25:
			// DCR H
			state->h -= 1;
			state->cc.cy = (state->h > 0xff);
			state->cc.z = ((state->h & 0xff) == 0);
			state->cc.s = ((state->h & 0x80) != 0);
			state->cc.p = parity(state->h, 8);
			break;
		case 0x26:
			// MVI H, d8
			state->h = opcode[1];
			state->pc += 1;
			break;
		case 0x27:
			// DAA
			if((state->a & 0xf) > 9){
				state->a += 6;
			}
			if((state->a & 0xf0) > 0x90){
				uint16_t answer = (uint16_t)state->a + 0x60;
				state->a = answer & 0xff;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
			}
			break;
		case 0x28:
			
			break;
		case 0x29:
			// DAD H
			{
			uint32_t hl = (state->h << 8) | (state->l);
			uint32_t answer = hl << 1;
			state->h = (uint8_t)(answer >> 8) & 0xff;
			state->l = (uint8_t)answer & 0xff;
			state->cc.cy = (answer > 0xffff);
			}
			break;
		case 0x2A:
			// LHDL d16
			{
			uint16_t offset = (opcode[2] << 8) | opcode[1];
			state->l = state->memory[offset];
			state->h = state->memory[offset+1];
			state->pc += 2;
			}
			break;
		case 0x2B:
			// DCX H
			state->l -= 1;
			if(state->l == 0xff){
				state->h -= 1;
			}
			break;
		case 0x2C:
			// INR L
			{
				uint16_t answer = state->l + 1;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				state->l = answer;
			}
			break;
		case 0x2D:
			// DCR L
			{
				uint16_t answer = state->l - 1;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				state->l = answer;
			}
			break;
		case 0x2E:
			// MVI L, d8
			state->l = opcode[1];
			state->pc += 1;
			break;
		case 0x2F:
			// CMA
			state->a = ~state->a;
			break;

		case 0x30: 
			
			break;
		case 0x31:
			// LXI SP, d16
			state->sp = (opcode[2] << 8) | opcode[1];
			state->pc += 2;
			break;
		case 0x32:
			// STA addr
			{
			uint16_t offset = (opcode[2] << 8) | opcode[1];
			write_ram(state, offset, state->a);
			state->pc += 2;
			}
			break;
		case 0x33:
			// INX SP
			state->sp++;
			break;
		case 0x34:
			// INR M
			{
				uint16_t offset = (state->h << 8) | state->l;
				uint8_t answer = state->memory[offset] + 1;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				write_ram(state, offset, answer);
			}
			break;
		case 0x35:
			// DCR M
			{	uint16_t offset = (state->h << 8) | (state->l);
				uint16_t answer = state->memory[offset] - 1;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				write_ram(state, offset, answer);
			}
			break;
		case 0x36:
			// MVI M, d8
			{
				uint32_t offset = (state->h << 8) | (state->l);
				write_ram(state, offset, opcode[1]);
				state->pc += 1;
			}
			break;
		case 0x37:
			// STC
			state->cc.cy = 1;
			break;
		case 0x38:
			
			break;
		case 0x39:
			//DAD SP
			{
				uint32_t hl = (state->h << 8) | state->l;
				uint32_t res = hl + state->sp;
				state->h = (res & 0xff00) >> 8;
				state->l = res & 0xff;
				state->cc.cy = ((res & 0xffff0000) > 0);
			}
			break;
		case 0x3A:
			// LDA addr
			{
			uint16_t offset = (opcode[2] << 8) | opcode[1];
			state->a = state->memory[offset];
			state->pc += 2;
			}
			break;
		case 0x3B:
			// DCX SP
			state->sp -= 1;
			break;
		case 0x3C:
			// INR A
			{
				uint16_t answer = state->a + 1;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				state->a = answer;
			}
			break;
		case 0x3D:
			// DCR A
			{
				uint16_t answer = state->a - 1;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				state->a = answer;
			}
			break;
		case 0x3E:
			// MVI A, d8
			state->a = opcode[1];
			state->pc += 1;
			break;		
		case 0x3F:
			// CMC
			state->cc.cy = 0;
			break;

		case 0x40: 
			break;
		case 0x41:
			// MOV B, C
			state->b = state->c;
			break;
		case 0x42:
			// MOV B, D
			state->b = state->d;
			break;
		case 0x43:
			// MOV B, E
			state->b = state->e;
			break;
		case 0x44:
			// MOV B, H
			state->b = state->h;
			break;
		case 0x45:
			// MOV B, L
			state->b = state->l;
			break;
		case 0x46:
			// MOV B, M
			{
			uint32_t offset = (state->h << 8) | (state->l);
			state->b = state->memory[offset];
			}
			break;
		case 0x47:
			// MOV B, A
			state->b = state->a;
			break;
		case 0x48:
			// MOV C, B
			state->c = state->b;
			break;
		case 0x49:
			// MOV C, C
			break;
		case 0x4A:
			// MOV C, D
			state->c = state->d;
			break;
		case 0x4B:
			// MOV C, E
			state->c = state->e;
			break;
		case 0x4C:
			// MOV C, H
			state->c = state->h;
			break;
		case 0x4D:
			// MOV C, L
			state->c = state->l;
			break;
		case 0x4E:
			// MOV C, M
			{
			uint32_t offset = (state->h << 8) | (state->l);
			state->c = state->memory[offset];
			}
			break;	
		case 0x4F:
			// MOV C, A
			state->c = state->a;
			break;

		case 0x50: 
			// MOV D, B
			state->d = state->b;
			break;
		case 0x51:
			// MOV D, C
			state->d = state->c;
			break;
		case 0x52:
			// MOV D, D
			break;
		case 0x53:
			// MOV D, E
			state->d = state->e;
			break;
		case 0x54:
			// MOV D, H
			state->d = state->h;
			break;
		case 0x55:
			// MOV D, L
			state->d = state->l;
			break;
		case 0x56:
			// MOV D, M
			{
			uint32_t offset = (state->h << 8) | (state->l);
			state->d = state->memory[offset];
			}
			break;
		case 0x57:
			// MOV D, A
			state->d = state->a;
			break;
		case 0x58:
			// MOV E, B
			state->e = state->b;
			break;
		case 0x59:
			// MOV E, C
			state->e = state->c;
			break;
		case 0x5A:
			// MOV E, D
			state->e = state->d;
			break;
		case 0x5B:
			// MOV E, E
			break;
		case 0x5C:
			// MOV E, H
			state->e = state->h;
			break;
		case 0x5D:
			// MOV E, L
			state->e = state->l;
			break;
		case 0x5E:
			// MOV E, M
			{
			uint32_t offset = (state->h << 8) | (state->l);
			state->e = state->memory[offset];
			}
			break;	
		case 0x5F:
			// MOV E, A
			state->e = state->a;
			break;

		case 0x60: 
			// MOV H, B
			state->h = state->b;
			break;
		case 0x61:
			// MOV H, C
			state->h = state->c;
			break;
		case 0x62:
			// MOV H, D
			state->h = state->d;
			break;
		case 0x63:
			// MOV H, E
			state->h = state->e;
			break;
		case 0x64:
			// MOV H, H
			break;
		case 0x65:
			// MOV H, L
			state->h = state->l;
			break;
		case 0x66:
			// MOV H, M
			{
			uint32_t offset = (state->h << 8) | (state->l);
			state->h = state->memory[offset];
			}
			break;
		case 0x67:
			// MOV H, A
			state->h = state->a;
			break;
		case 0x68:
			// MOV L, B
			state->l = state->b;
			break;
		case 0x69:
			// MOV L, C
			state->l = state->c;
			break;
		case 0x6A:
			// MOV L, D
			state->l = state->d;
			break;
		case 0x6B:
			// MOV L, E
			state->l = state->e;
			break;
		case 0x6C:
			// MOV L, H
			state->l = state->h;
			break;
		case 0x6D:
			// MOV L, L
			break;
		case 0x6E:
			// MOV L, M
			{
			uint32_t offset = (state->h << 8) | (state->l);
			state->l = state->memory[offset];
			}
			break;	
		case 0x6F:
			// MOV L, A
			state->l = state->a;
			break;

		case 0x70: 
			// MOV M, B
			{
			uint32_t offset = (state->h << 8) | (state->l);
			write_ram(state, offset, state->b);
			}
			break;
		case 0x71:
			// MOV M, C
			{
			uint32_t offset = (state->h << 8) | (state->l);
			write_ram(state, offset, state->c);
			}
			break;
		case 0x72:
			// MOV M, D
			{
			uint32_t offset = (state->h << 8) | (state->l);
			write_ram(state, offset, state->d);
			}
			break;
		case 0x73:
			// MOV M, E
			{
			uint32_t offset = (state->h << 8) | (state->l);
			write_ram(state, offset, state->e);
			}
			break;
		case 0x74:
			// MOV M, H
			{
			uint32_t offset = (state->h << 8) | (state->l);
			write_ram(state, offset, state->h);
			}
			break;
		case 0x75:
			// MOV M, L
			{
			uint32_t offset = (state->h << 8) | (state->l);
			write_ram(state, offset, state->l);
			}
			break;
		case 0x76:
			// HLT
			break;
		case 0x77:
			// MOV M, A
			{
			uint32_t offset = (state->h << 8) | (state->l);
			write_ram(state, offset, state->a);
			}
			break;
		case 0x78:
			// MOV A, B
			state->a = state->b;
			break;break;
		case 0x79:
			// MOV A, C
			state->a = state->c;
			break;
		case 0x7A:
			// MOV A, D
			state->a = state->d;
			break;
		case 0x7B:
			// MOV A, E
			state->a = state->e;
			break;
		case 0x7C:
			// MOV A, H
			state->a = state->h;
			break;
		case 0x7D:
			// MOV A, L
			state->a = state->l;
			break;
		case 0x7E:
			// MOV A, M
			{
			uint32_t offset = (state->h << 8) | (state->l);
			state->a = state->memory[offset];
			}
			break;	
		case 0x7F:
			break;

		case 0x80: 
			// ADD B
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->b;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}break;
		case 0x81:
			// ADD C
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->c;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x82:
			// ADD D
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->d;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x83:
			// ADD E
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->e;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x84:
			// ADD H
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->h;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}break;
		case 0x85:
			// ADD L
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->l;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x86:
			// ADD M
			{
			uint16_t offset = (state->h << 8) | state->l;
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->memory[offset];
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x87:
			// ADD A
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->a;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x88:
			// ADC B
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->b + state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x89:
			// ADC C
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->c + state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x8A:
			// ADC D
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->d + state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x8B:
			// ADC E
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->e + state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x8C:
			// ADC H
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->h + state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x8D:
			// ADC L
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->l + state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x8E:
			// ADC M
			{
			uint16_t offset = (state->h << 8) | state->l;
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->memory[offset] + state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;	
		case 0x8F:
			// ADC A
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)state->a + state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;


		case 0x90: 
			// SUB B
			{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->b;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x91:
			// SUB C
			{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->c;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x92:
			// SUB D
			{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->d;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x93:
			// SUB E
					{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->e;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x94:
			// SUB H
					{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->h;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x95:
			// SUB L
					{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->l;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x96:
			// SUB M
					{
			uint16_t offset = (state->h << 8) | state->l;
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->memory[offset];
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x97:
			// SUB A
					{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->a;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x98:
			// SBB B
				{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->b - state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x99:
			// SBB C
				{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->c - state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x9A:
			// SBB D
				{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->d - state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x9B:
			// SBB E
				{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->e - state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x9C:
			// SBB H
				{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->h - state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x9D:
			// SBB L
				{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->l - state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;
		case 0x9E:
			// SBB M
				{
			uint16_t offset = (state->h << 8) | state->l;
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->memory[offset] - state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;	
		case 0x9F:
			// SBB A
				{
			uint16_t answer = (uint16_t)state->a - (uint16_t)state->a - state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			}
			break;

		case 0xA0: 
			// ANA B
			state->a = state->a & state->b;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a , 8);
			break;
		case 0xA1:
			// ANA C
			state->a = state->a & state->c;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a , 8);
			break;
		case 0xA2:
			// ANA D
			state->a = state->a & state->d;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a , 8);
			break;
		case 0xA3:
			// ANA E
			state->a = state->a & state->e;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a , 8);
			break;
		case 0xA4:
			// ANA H
			state->a = state->a & state->h;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a , 8);
			break;
		case 0xA5:
			// ANA L
			state->a = state->a & state->l;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a , 8);
			break;
		case 0xA6:
			// ANA M
			{
			uint16_t offset = (state->h << 8) | state->l;
			state->a = state->a & state->memory[offset];
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a , 8);
			}
			break;
		case 0xA7:
			// ANA A
			state->a = state->a & state->a;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a , 8);
			break;
		case 0xA8:
			// XRA B
			state->a = state->a ^ state->b;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xA9:
			// XRA C
			state->a = state->a ^ state->c;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xAA:
			// XRA D
			state->a = state->a ^ state->d;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xAB:
			// XRA E
			state->a = state->a ^ state->e;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xAC:
			// XRA H
			state->a = state->a ^ state->h;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xAD:
			// XRA L
			state->a = state->a ^ state->l;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xAE:
			// XRA M
			{
			uint16_t offset = (state->h << 8) | state->l;
			state->a = state->a ^ state->memory[offset];
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a , 8);
			}
			break;	
		case 0xAF:
			// XRA A
			state->a = state->a ^ state->a;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;

		case 0xB0: 
			// ORA B
			state->a = state->a | state->b;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xB1:
			// ORA C
			state->a = state->a | state->c;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xB2:
			// ORA D
			state->a = state->a | state->d;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xB3:
			// ORA E
			state->a = state->a | state->e;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xB4:
			// ORA H
			state->a = state->a | state->h;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xB5:
			// ORA L
			state->a = state->a | state->l;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xB6:
			// ORA M
			{
			uint16_t offset = ((state->h) << 8)| state->l;
			state->a = state->a | state->memory[offset];
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			}
			break;
		case 0xB7:
			// ORA A
			state->a = state->a | state->a;
			state->cc.cy = 0;
			state->cc.z = ((state->a & 0xff) == 0);
			state->cc.s = ((state->a & 0x80) != 0);
			state->cc.p = parity(state->a, 8);
			break;
		case 0xB8:
			// CMP B
			{
				uint16_t answer = (uint16_t)state->a - (uint16_t)state->b;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				
			}
			break;
		case 0xB9:
			// CMP C
			{
				uint16_t answer = (uint16_t)state->a - (uint16_t)state->c;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				
			}
			break;
		case 0xBA:
			// CMP D
			{
				uint16_t answer = (uint16_t)state->a - (uint16_t)state->d;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				
			}
			break;
		case 0xBB:
			// CMP E
			{
				uint16_t answer = (uint16_t)state->a - (uint16_t)state->e;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				
			}
			break;
		case 0xBC:
			// CMP H
			{
				uint16_t answer = (uint16_t)state->a - (uint16_t)state->h;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				
			}
			break;
		case 0xBD:
			// CMP L
			{
				uint16_t answer = (uint16_t)state->a - (uint16_t)state->l;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				
			}
			break;
		case 0xBE:
			// CMP M
			{
				uint16_t offset = ((state->h) << 8)| state->l;
				uint16_t answer = (uint16_t)state->a - (uint16_t)state->memory[offset];
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				
			}
			break;	
		case 0xBF:
			// CMP A
			{
				uint16_t answer = (uint16_t)state->a - (uint16_t)state->a;
				state->cc.cy = (answer > 0xff);
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				
			}
			break;

		case 0xC0: 
			// RNZ
			if(state->cc.z == 0){
				state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
				state->sp += 2;
			}
			break;
		case 0xC1:
			// POP B
			pop(state, &state->b, &state->c);
			break;
		case 0xC2:
			// JNZ addr
			if(state->cc.z == 0){
				state->pc = (opcode[2] << 8) | opcode[1];
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xC3:
			// JMP addr
			state->pc = (opcode[2] << 8) | opcode[1];
			break;
		case 0xC4:
			// CNZ addr
			if(state->cc.z == 0){
				uint16_t offset = (opcode[2] << 8) | opcode[1];
				uint16_t ret = state->pc + 2;
				write_ram(state, state->sp-1, (ret >> 8) & 0xff);
				write_ram(state, state->sp-2, ret & 0xff);
				state->sp = state->sp - 2;
				state->pc = offset;
			
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xC5:
			// PUSH B
			push(state, state->b, state->c);
			break;
		case 0xC6:
			// ADI d8
			{
			uint16_t answer = (uint16_t)state->a + (uint16_t)opcode[1];
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			state->pc += 1;
			}
			break;
		case 0xC7:
			// RST 0
		{
			uint16_t ret = state->pc+2;
			write_ram(state, state->sp-1, (ret >> 8) & 0xff);
			write_ram(state, state->sp-2, ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = 0x0000;
		}
			break;
		case 0xC8:
			// RZ
			if(state->cc.z){
				state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
				state->sp += 2;
			}
			break;
		case 0xC9:
			// RET
			state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
			state->sp += 2;
			break;
		case 0xCA:
			// JZ addr
			if(state->cc.z){
				state->pc = (opcode[2] << 8) | opcode[1];
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xCB:
			// JMP
			state->pc = (opcode[2] << 8) | opcode[1];
			break;
		case 0xCC:
			// CZ addr
			if(state->cc.z == 1){
			
				uint16_t offset = (opcode[2] << 8) | opcode[1];
				uint16_t ret = state->pc + 2;
				write_ram(state, state->sp-1, (ret >> 8) & 0xff);
				write_ram(state, state->sp-2, ret & 0xff);
				state->sp = state->sp - 2;
				state->pc = offset;
			
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xCD:
			// CALL addr
			{
			uint16_t offset = (opcode[2] << 8) | opcode[1];
			uint16_t ret = state->pc + 2;
			write_ram(state, state->sp-1, (ret >> 8) & 0xff);
			write_ram(state, state->sp-2, ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = offset;
			}
			break;
		case 0xCE:
			// ACI d8
		{
			uint16_t answer = state->a + opcode[1] + state->cc.cy;
			state->cc.cy = (answer > 0xff);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = answer & 0xff;
			state->pc++;
		}
			break;
		case 0xCF:
			// RST 1
		{
			uint16_t ret = state->pc+2;
			write_ram(state, state->sp-1, (ret >> 8) & 0xff);
			write_ram(state, state->sp-2, ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = 0x0008;
		}
			break;

		case 0xD0: 
			// RNC
			if(state->cc.cy == 0){
				state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
				state->sp += 2;
			}
			break;
		case 0xD1:
			// POP D
			pop(state, &state->d, &state->e);
			break;
		case 0xD2:
			// JNC d16
			if(state->cc.cy == 0){
				state->pc = (opcode[2] << 8) | opcode[1];
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xD3:
			// OUT d8
			// COMPLETE HERE !!

			state->pc += 1;
			break;
		case 0xD4:
			// CNC d16
			if(state->cc.cy == 0){
			
				uint16_t offset = (opcode[2] << 8) | opcode[1];
				uint16_t ret = state->pc + 2;
				write_ram(state, state->sp-1, (ret >> 8) & 0xff);
				write_ram(state, state->sp-2, ret & 0xff);
				state->sp = state->sp - 2;
				state->pc = offset;
			
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xD5:
			// PUSH D
			push(state, state->d, state->e);
			break;
		case 0xD6:
			// SUI d8
		{
			uint8_t answer = state->a - opcode[1];
			state->cc.cy = (state->a < opcode[1]);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = answer;
			state->pc++;
		}
			break;
		case 0xD7:
			// RST 2
		{
			uint16_t ret = state->pc+2;
			write_ram(state, state->sp-1, (ret >> 8) & 0xff);
			write_ram(state, state->sp-2, ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = 0x10;
		}
			break;
		case 0xD8:
			// RC
			if(state->cc.cy != 0){
				state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
				state->sp += 2;
			}
			break;
		case 0xD9:
			
			break;
		case 0xDA:
			// JC addr
			if(state->cc.cy != 0){
				state->pc = (opcode[2] << 8) | opcode[1];
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xDB:
			// IN
			// COMPLETE HERE !!

			state->pc += 1;
			break;
		case 0xDC:
			// CC addr
			if(state->cc.cy != 0){
			
				uint16_t offset = (opcode[2] << 8) | opcode[1];
				uint16_t ret = state->pc + 2;
				write_ram(state, state->sp-1, (ret >> 8) & 0xff);
				write_ram(state, state->sp-2, ret & 0xff);
				state->sp = state->sp - 2;
				state->pc = offset;
			
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xDD:
			
			break;
		case 0xDE:
			// SBI d8
			{
				uint16_t answer = state->a - opcode[1] - state->cc.cy;
				state->cc.z = ((answer&0xff) == 0);
				state->cc.s = (0x80 == ((answer&0xff) & 0x80));
				state->cc.p = parity((answer&0xff), 8);
				state->cc.cy = (answer > 0xff);
				state->a = answer & 0xff;
				state->pc++;

			}
			break;
		case 0xDF:
			// RST 3
		{
			uint16_t ret = state->pc+2;
			write_ram(state, state->sp-1, (ret >> 8) & 0xff);
			write_ram(state, state->sp-2, ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = 0x18;
		}
			break;

		case 0xE0: 
			// RPO
			if(state->cc.p == 0){
				state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
				state->sp += 2;
			}
			break;
		case 0xE1:
			// POP H
			pop(state, &state->h, &state->l);
			break;
		case 0xE2:
			// JPO
			if(state->cc.p == 0){
				state->pc = (opcode[2] << 8) | opcode[1];
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xE3:
			// XTHL
			{
			uint8_t h = state->h;
			uint8_t l = state->l;
			state->l = state->memory[state->sp];
			state->h = state->memory[state->sp + 1];
			write_ram(state, state->sp, l);
			write_ram(state, state->sp+1, h);
			}
			break;
		case 0xE4:
			// CPO addr
			if(state->cc.p == 0){
				uint16_t offset = (opcode[2] << 8) | opcode[1];
				uint16_t ret = state->pc + 2;
				write_ram(state, state->sp-1, (ret >> 8) & 0xff);
				write_ram(state, state->sp-2, ret & 0xff);
				state->sp = state->sp - 2;
				state->pc = offset;
			
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xE5:
			// PUSH H
			push(state, state->h, state->l);
			break;
		case 0xE6:
			// ANI d8
			{
			uint16_t answer = (uint16_t)state->a & (uint16_t)opcode[1];
			state->cc.cy = 0;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = (uint8_t)answer;
			state->pc += 1;
			}
			break;
		case 0xE7:
			// RST 4
		{
			uint16_t ret = state->pc+2;
			write_ram(state, state->sp-1, (ret >> 8) & 0xff);
			write_ram(state, state->sp-2, ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = 0x20;
		}
			break;
		case 0xE8:
			// RPE
			if(state->cc.p != 0){
				state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
				state->sp += 2;
			}
			break;
		case 0xE9:
			// PCHL
			{
				state->pc = (state->h << 8) | (state->l);
			}
			break;
		case 0xEA:
			// JPE
			if(state->cc.p != 0){
				state->pc = (opcode[2] << 8) | opcode[1];
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xEB:
			// XCHG
			{
			uint8_t temp = state->d;
			state->d = state->h;
			state->h = temp;

			temp = state->e;
			state->e = state->l;
			state->l = temp;
			}
			break;
		case 0xEC:
			// CPE addr
			if(state->cc.p != 0){
				uint16_t offset = (opcode[2] << 8) | opcode[1];
				uint16_t ret = state->pc + 2;
				write_ram(state, state->sp-1, (ret >> 8) & 0xff);
				write_ram(state, state->sp-2, ret & 0xff);
				state->sp = state->sp - 2;
				state->pc = offset;
			
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xED:
			
			break;
		case 0xEE:
			// XRI data
		{
			uint8_t answer = state->a ^ opcode[1];
			state->cc.cy = 0;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->a = answer;
			state->pc++;
		}
			break;
		case 0xEF:
			// RST 5
		{
			uint16_t ret = state->pc+2;
			write_ram(state, state->sp-1, (ret >> 8) & 0xff);
			write_ram(state, state->sp-2, ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = 0x28;
		}
			break;

		case 0xF0: 
			// RP
			if(state->cc.s == 0){
				state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
				state->sp += 2;
			}
			break;
		case 0xF1:
			// POP PSW
			pop(state, &state->a, (uint8_t*)&state->cc);
			break;
		case 0xF2:
			// JP addr
			if(state->cc.s == 0){
				state->pc = (opcode[2] << 8) | opcode[1];
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xF3:
			// DI
			state->int_enable = 0;
			break;
		case 0xF4:
			// CP
			if(state->cc.s == 0){
			
				uint16_t offset = (opcode[2] << 8) | opcode[1];
				uint16_t ret = state->pc + 2;
				write_ram(state, state->sp-1, (ret >> 8) & 0xff);
				write_ram(state, state->sp-2, ret & 0xff);
				state->sp = state->sp - 2;
				state->pc = offset;
			
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xF5:
			// PUSH PSW
			push(state, state->a, *(uint8_t*)&state->cc);
			break;
		case 0xF6:
			// ORI d8
			{
				uint8_t answer = state->a | opcode[1];
				state->cc.cy = 0;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = parity(answer, 8);
				state->a = answer;
				state->pc++;
			}
			break;
		case 0xF7:
			// RST 6
		{
			uint16_t ret = state->pc+2;
			write_ram(state, state->sp-1, (ret >> 8) & 0xff);
			write_ram(state, state->sp-2, ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = 0x30;
		}
			break;
		case 0xF8:
			// RM
			if(state->cc.s != 0){
				state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
				state->sp += 2;
			}
			break;
		case 0xF9:
			// SPHL
			state->sp = state->l | (state->h << 8);
			break;
		case 0xFA:
			// JM
			
			if(state->cc.s != 0){
				uint16_t offset = (opcode[2] << 8) | opcode[1];
				state->pc = offset;
			}
			else{
				state->pc += 2;
			}
			
			break;
		case 0xFB:
			// EI
			state->int_enable = 1;
			break;
		case 0xFC:
			// CM d16
			if(state->cc.s != 0){
			
				uint16_t offset = (opcode[2] << 8) | opcode[1];
				uint16_t ret = state->pc + 2;
				write_ram(state, state->sp-1, (ret >> 8) & 0xff);
				write_ram(state, state->sp-2, ret & 0xff);
				state->sp = state->sp - 2;
				state->pc = offset;
			
			}
			else{
				state->pc += 2;
			}
			break;
		case 0xFD:
			
			break;
		case 0xFE:
			// CPI d8
			{
			uint8_t answer = state->a - opcode[1];
			state->cc.cy = (state->a < opcode[1]);
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = parity(answer, 8);
			state->pc += 1;
			}
			break;
		case 0xFF:
			// RST 7
			{
			uint16_t ret = state->pc+2;
			write_ram(state, state->sp-1, (ret >> 8) & 0xff);
			write_ram(state, state->sp-2, ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = 0x38;
			}
			break;
	}

#if PRINTOP

	//printf("\x1B[2J\x1B[H");
	printf("A: %02x B: %02x C: %02x D: %02x E: %02x H: %02x L: %02x\t", state->a, state->b, state->c, 
			state->d, state->e, state->h, state->l);
	printf("%c", state->cc.z ? 'z' : '.');
	printf("%c", state->cc.s ? 's' : '.');
	printf("%c", state->cc.p ? 'p' : '.');
	printf("%c", state->cc.cy ? 'c' : '.');
	printf("\tSP: %04x	PC: %04x\n", state->sp, state->pc);

#endif

	return cycles8080[*opcode];
}


void generate_interrupt(state_8080 *state, uint32_t interrupt_number){
	// push PC
	push(state, (state->pc & 0xFF00) >> 8, (state->pc & 0xFF));

	// set PC to low memory function
	// equivalent to RST
	state->pc = 8 * interrupt_number;

	state->int_enable = 0;
}

