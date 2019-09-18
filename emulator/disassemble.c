#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t disassemble8080op(uint8_t *buffer, uint32_t pc){
	uint8_t *op = buffer + pc;
	uint32_t opbytes = 1;
	printf("%04x ", pc);

	switch(*op){
		case 0x00: 
			printf("NOP"); break;
		case 0x01:
			printf("LXI B, #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0x02:
			printf("STAX B"); break;
		case 0x03:
			printf("INX B"); break;
		case 0x04:
			printf("INR B"); break;
		case 0x05:
			printf("DCR B"); break;
		case 0x06:
			printf("MVI B, #$%02x", op[1]);
			opbytes = 2; break;
		case 0x07:
			printf("RLC"); break;
		case 0x08:
			printf("NOP"); break;
		case 0x09:
			printf("DAD B"); break;
		case 0x0A:
			printf("LADX B"); break;
		case 0x0B:
			printf("DCX B"); break;
		case 0x0C:
			printf("INR C"); break;
		case 0x0D:
			printf("DCR C"); break;
		case 0x0E:
			printf("MVI C, #$%02x", op[1]);
			opbytes = 2; break;
		case 0x0F:
			printf("RRC"); break;

		case 0x10: 
			printf("NOP"); break;
		case 0x11:
			printf("LXI D, #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0x12:
			printf("STAX D"); break;
		case 0x13:
			printf("INX D"); break;
		case 0x14:
			printf("INR D"); break;
		case 0x15:
			printf("DCR D"); break;
		case 0x16:
			printf("MVI D, #$%02x", op[1]);
			opbytes = 2; break;
		case 0x17:
			printf("RAL"); break;
		case 0x18:
			printf("NOP"); break;
		case 0x19:
			printf("DAD D"); break;
		case 0x1A:
			printf("LADX D"); break;
		case 0x1B:
			printf("DCX D"); break;
		case 0x1C:
			printf("INR E"); break;
		case 0x1D:
			printf("DCR E"); break;
		case 0x1E:
			printf("MVI E, #$%02x", op[1]);
			opbytes = 2; break;
		case 0x1F:
			printf("RAR"); break;

		case 0x20: 
			printf("NOP"); break;
		case 0x21:
			printf("LXI H, #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0x22:
			printf("SHLD #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0x23:
			printf("INX H"); break;
		case 0x24:
			printf("INR H"); break;
		case 0x25:
			printf("DCR H"); break;
		case 0x26:
			printf("MVI H, #$%02x", op[1]);
			opbytes = 2; break;
		case 0x27:
			printf("DAA"); break;
		case 0x28:
			printf("NOP"); break;
		case 0x29:
			printf("DAD H"); break;
		case 0x2A:
			printf("LHDL #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0x2B:
			printf("DCX H"); break;
		case 0x2C:
			printf("INR L"); break;
		case 0x2D:
			printf("DCR L"); break;
		case 0x2E:
			printf("MVI L, #$%02x", op[1]);
			opbytes = 2; break;
		case 0x2F:
			printf("CMA"); break;

		case 0x30: 
			printf("NOP"); break;
		case 0x31:
			printf("LXI SP, #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0x32:
			printf("STA #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0x33:
			printf("INX SP"); break;
		case 0x34:
			printf("INR M"); break;
		case 0x35:
			printf("DCR M"); break;
		case 0x36:
			printf("MVI M, #$%02x", op[1]);
			opbytes = 2; break;
		case 0x37:
			printf("STC"); break;
		case 0x38:
			printf("NOP"); break;
		case 0x39:
			printf("DAD SP"); break;
		case 0x3A:
			printf("LDA #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0x3B:
			printf("DCX SP"); break;
		case 0x3C:
			printf("INR A"); break;
		case 0x3D:
			printf("DCR A"); break;
		case 0x3E:
			printf("MVI A, #$%02x", op[1]);
			opbytes = 2; break;		
		case 0x3F:
			printf("CMC"); break;

		case 0x40: 
			printf("MOV B, B"); break;
		case 0x41:
			printf("MOV B, C"); break;
		case 0x42:
			printf("MOV B, D"); break;
		case 0x43:
			printf("MOV B, E"); break;
		case 0x44:
			printf("MOV B, H"); break;
		case 0x45:
			printf("MOV B, L"); break;
		case 0x46:
			printf("MOV B, M"); break;
		case 0x47:
			printf("MOV B, A"); break;
		case 0x48:
			printf("MOV C, B"); break;
		case 0x49:
			printf("MOV C, C"); break;
		case 0x4A:
			printf("MOV C, D"); break;
		case 0x4B:
			printf("MOV C, E"); break;
		case 0x4C:
			printf("MOV C, H"); break;
		case 0x4D:
			printf("MOV C, L"); break;
		case 0x4E:
			printf("MOV C, M"); break;	
		case 0x4F:
			printf("MOV C, A"); break;

		case 0x50: 
			printf("MOV D, B"); break;
		case 0x51:
			printf("MOV D, C"); break;
		case 0x52:
			printf("MOV D, D"); break;
		case 0x53:
			printf("MOV D, E"); break;
		case 0x54:
			printf("MOV D, H"); break;
		case 0x55:
			printf("MOV D, L"); break;
		case 0x56:
			printf("MOV D, M"); break;
		case 0x57:
			printf("MOV D, A"); break;
		case 0x58:
			printf("MOV E, B"); break;
		case 0x59:
			printf("MOV E, C"); break;
		case 0x5A:
			printf("MOV E, D"); break;
		case 0x5B:
			printf("MOV E, E"); break;
		case 0x5C:
			printf("MOV E, H"); break;
		case 0x5D:
			printf("MOV E, L"); break;
		case 0x5E:
			printf("MOV E, M"); break;	
		case 0x5F:
			printf("MOV E, A"); break;

		case 0x60: 
			printf("MOV H, B"); break;
		case 0x61:
			printf("MOV H, C"); break;
		case 0x62:
			printf("MOV H, D"); break;
		case 0x63:
			printf("MOV H, E"); break;
		case 0x64:
			printf("MOV H, H"); break;
		case 0x65:
			printf("MOV H, L"); break;
		case 0x66:
			printf("MOV H, M"); break;
		case 0x67:
			printf("MOV H, A"); break;
		case 0x68:
			printf("MOV L, B"); break;
		case 0x69:
			printf("MOV L, C"); break;
		case 0x6A:
			printf("MOV L, D"); break;
		case 0x6B:
			printf("MOV L, E"); break;
		case 0x6C:
			printf("MOV L, H"); break;
		case 0x6D:
			printf("MOV L, L"); break;
		case 0x6E:
			printf("MOV L, M"); break;	
		case 0x6F:
			printf("MOV L, A"); break;

		case 0x70: 
			printf("MOV M, B"); break;
		case 0x71:
			printf("MOV M, C"); break;
		case 0x72:
			printf("MOV M, D"); break;
		case 0x73:
			printf("MOV M, E"); break;
		case 0x74:
			printf("MOV M, H"); break;
		case 0x75:
			printf("MOV M, L"); break;
		case 0x76:
			printf("HLT"); break;
		case 0x77:
			printf("MOV M, A"); break;
		case 0x78:
			printf("MOV A, B"); break;
		case 0x79:
			printf("MOV A, C"); break;
		case 0x7A:
			printf("MOV A, D"); break;
		case 0x7B:
			printf("MOV A, E"); break;
		case 0x7C:
			printf("MOV A, H"); break;
		case 0x7D:
			printf("MOV A, L"); break;
		case 0x7E:
			printf("MOV A, M"); break;	
		case 0x7F:
			printf("MOV A, A"); break;

		case 0x80: 
			printf("ADD B"); break;
		case 0x81:
			printf("ADD C"); break;
		case 0x82:
			printf("ADD D"); break;
		case 0x83:
			printf("ADD E"); break;
		case 0x84:
			printf("ADD H"); break;
		case 0x85:
			printf("ADD L"); break;
		case 0x86:
			printf("ADD M"); break;
		case 0x87:
			printf("ADD A"); break;
		case 0x88:
			printf("ADC B"); break;
		case 0x89:
			printf("ADC C"); break;
		case 0x8A:
			printf("ADC D"); break;
		case 0x8B:
			printf("ADC E"); break;
		case 0x8C:
			printf("ADC H"); break;
		case 0x8D:
			printf("ADC L"); break;
		case 0x8E:
			printf("ADC M"); break;	
		case 0x8F:
			printf("ADC A"); break;


		case 0x90: 
			printf("SUB B"); break;
		case 0x91:
			printf("SUB C"); break;
		case 0x92:
			printf("SUB D"); break;
		case 0x93:
			printf("SUB E"); break;
		case 0x94:
			printf("SUB H"); break;
		case 0x95:
			printf("SUB L"); break;
		case 0x96:
			printf("SUB M"); break;
		case 0x97:
			printf("SUB A"); break;
		case 0x98:
			printf("SBB B"); break;
		case 0x99:
			printf("SBB C"); break;
		case 0x9A:
			printf("SBB D"); break;
		case 0x9B:
			printf("SBB E"); break;
		case 0x9C:
			printf("SBB H"); break;
		case 0x9D:
			printf("SBB L"); break;
		case 0x9E:
			printf("SBB M"); break;	
		case 0x9F:
			printf("SBB A"); break;

		case 0xA0: 
			printf("ANA B"); break;
		case 0xA1:
			printf("ANA C"); break;
		case 0xA2:
			printf("ANA D"); break;
		case 0xA3:
			printf("ANA E"); break;
		case 0xA4:
			printf("ANA H"); break;
		case 0xA5:
			printf("ANA L"); break;
		case 0xA6:
			printf("ANA M"); break;
		case 0xA7:
			printf("ANA A"); break;
		case 0xA8:
			printf("XRA B"); break;
		case 0xA9:
			printf("XRA C"); break;
		case 0xAA:
			printf("XRA D"); break;
		case 0xAB:
			printf("XRA E"); break;
		case 0xAC:
			printf("XRA H"); break;
		case 0xAD:
			printf("XRA L"); break;
		case 0xAE:
			printf("XRA M"); break;	
		case 0xAF:
			printf("XRA A"); break;

		case 0xB0: 
			printf("ORA B"); break;
		case 0xB1:
			printf("ORA C"); break;
		case 0xB2:
			printf("ORA D"); break;
		case 0xB3:
			printf("ORA E"); break;
		case 0xB4:
			printf("ORA H"); break;
		case 0xB5:
			printf("ORA L"); break;
		case 0xB6:
			printf("ORA M"); break;
		case 0xB7:
			printf("ORA A"); break;
		case 0xB8:
			printf("CMP B"); break;
		case 0xB9:
			printf("CMP C"); break;
		case 0xBA:
			printf("CMP D"); break;
		case 0xBB:
			printf("CMP E"); break;
		case 0xBC:
			printf("CMP H"); break;
		case 0xBD:
			printf("CMP L"); break;
		case 0xBE:
			printf("CMP M"); break;	
		case 0xBF:
			printf("CMP A"); break;

		case 0xC0: 
			printf("RNZ"); break;
		case 0xC1:
			printf("POP B"); break;
		case 0xC2:
			printf("JNZ #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xC3:
			printf("JMP #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xC4:
			printf("CNZ #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xC5:
			printf("PUSH B"); break;
		case 0xC6:
			printf("ADI #$%02x", op[1]);
			opbytes = 2; break;
		case 0xC7:
			printf("RST 0"); break;
		case 0xC8:
			printf("RZ"); break;
		case 0xC9:
			printf("RET"); break;
		case 0xCA:
			printf("JZ #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xCB:
			printf("JMP #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xCC:
			printf("CZ #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xCD:
			printf("CALL #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xCE:
			printf("ACI #$%02x", op[1]);
			opbytes = 2; break;
		case 0xCF:
			printf("RST 1"); break;

		case 0xD0: 
			printf("RNC"); break;
		case 0xD1:
			printf("POP D"); break;
		case 0xD2:
			printf("JNC #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xD3:
			printf("OUT #$%02x", op[1]);
			opbytes = 2; break;
		case 0xD4:
			printf("CNC #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xD5:
			printf("PUSH D"); break;
		case 0xD6:
			printf("SUI #$%02x", op[1]);
			opbytes = 2; break;
		case 0xD7:
			printf("RST 2"); break;
		case 0xD8:
			printf("RC"); break;
		case 0xD9:
			printf("RET"); break;
		case 0xDA:
			printf("JC #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xDB:
			printf("IN #$%02x", op[1]);
			opbytes = 2; break;
		case 0xDC:
			printf("CC #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xDD:
			printf("CALL #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xDE:
			printf("SBI #$%02x", op[1]);
			opbytes = 2; break;
		case 0xDF:
			printf("RST 3"); break;

		case 0xE0: 
			printf("RPO"); break;
		case 0xE1:
			printf("POP H"); break;
		case 0xE2:
			printf("JPO #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xE3:
			printf("XTHL"); break;
		case 0xE4:
			printf("CPO #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xE5:
			printf("PUSH H"); break;
		case 0xE6:
			printf("ANI #$%02x", op[1]);
			opbytes = 2; break;
		case 0xE7:
			printf("RST 4"); break;
		case 0xE8:
			printf("RPE"); break;
		case 0xE9:
			printf("PCHL"); break;
		case 0xEA:
			printf("JPE #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xEB:
			printf("XCHG"); break;
		case 0xEC:
			printf("CPE #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xED:
			printf("CALL #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xEE:
			printf("XRI #$%02x", op[1]);
			opbytes = 2; break;
		case 0xEF:
			printf("RST 5"); break;

		case 0xF0: 
			printf("RP"); break;
		case 0xF1:
			printf("POP PSW"); break;
		case 0xF2:
			printf("JP #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xF3:
			printf("DI"); break;
		case 0xF4:
			printf("CP #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xF5:
			printf("PUSH PSW"); break;
		case 0xF6:
			printf("ORI #$%02x", op[1]);
			opbytes = 2; break;
		case 0xF7:
			printf("RST 6"); break;
		case 0xF8:
			printf("RM"); break;
		case 0xF9:
			printf("SPHL"); break;
		case 0xFA:
			printf("JM #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xFB:
			printf("EI"); break;
		case 0xFC:
			printf("CM #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xFD:
			printf("CALL #$%02x%02x", op[2], op[1]);
			opbytes = 3; break;
		case 0xFE:
			printf("CPI #$%02x", op[1]);
			opbytes = 2; break;
		case 0xFF:
			printf("RST 7"); break;
	}
	printf("\n");

	return opbytes;
}