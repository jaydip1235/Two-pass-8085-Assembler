#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include "../inc/simulator.h"

int parity(int x, int size)
{
	int i;
	int p = 0;
	x = (x & ((1 << size) - 1));
	for (i = 0; i < size; i++)
	{
		if (x & 0x1)
			p++;
		x = x >> 1;
	}
	return (0 == (p & 0x1));
}

void LogicFlagsA(State8085 *state, uint8_t ac)
{
	state->cc.cy = 0;
	state->cc.ac = 0;
	state->cc.z = (state->a == 0);
	state->cc.s = (0x80 == (state->a & 0x80));
	state->cc.p = parity(state->a, 8);
}

void ArithFlagsA(State8085 *state, uint16_t res, should_preserve_carry preserveCarry)
{
	if (preserveCarry == UPDATE_CARRY)
		state->cc.cy = (res > 0xff);
	state->cc.z = ((res & 0xff) == 0);
	state->cc.s = (0x80 == (res & 0x80));
	state->cc.p = parity(res & 0xff, 8);
}

void UnimplementedInstruction(State8085 *state)
{
	//pc will have advanced one, so undo that
	printf("Error: Unimplemented instruction\n");
	state->pc--;
	exit(1);
}

void InvalidInstruction(State8085 *state)
{
	//pc will have advanced one, so undo that
	printf("Error: Invalid instruction\n");
	printf("PC: %x\n", state->pc);
	printf("Memory at PC: %x\n", state->memory[state->pc]);
	state->pc--;
	exit(1);
}

uint8_t addByte(State8085 *state, uint8_t lhs, uint8_t rhs, should_preserve_carry preserveCarry)
{
	uint16_t res = lhs + rhs;
	ArithFlagsA(state, res, preserveCarry);
	if((lhs & 0xf) + (rhs & 0xf) > 0xf)
		state->cc.ac = 1;
	return (uint8_t)res;
}

uint8_t addByteWithCarry(State8085 *state, uint8_t lhs, uint8_t rhs, should_preserve_carry preserveCarry)
{
	uint16_t res = lhs + rhs + (state->cc.cy ? 1 : 0);
	ArithFlagsA(state, res, preserveCarry);
	return (uint8_t)res;
}

uint8_t subtractByte(State8085 *state, uint8_t lhs, uint8_t rhs, should_preserve_carry preserveCarry)
{
	uint16_t res = lhs - rhs;
	ArithFlagsA(state, res, preserveCarry);
	if((lhs & 0xf) + (~rhs & 0xf) + 1 > 0xf)
		state->cc.ac = 1;
	return (uint8_t)res;
}

uint8_t subtractByteWithBorrow(State8085 *state, uint8_t lhs, uint8_t rhs, should_preserve_carry preserveCarry)
{
	uint16_t res = lhs - rhs - (state->cc.cy ? 1 : 0);
	ArithFlagsA(state, res, preserveCarry);
	return (uint8_t)res;
}

void call(State8085 *state, uint16_t addr)
{
	uint16_t pc = state->pc + 2;
	state->memory[state->sp - 1] = (pc >> 8) & 0xff;
	state->memory[state->sp - 2] = (pc & 0xff);
	state->sp = state->sp - 2;
	// state->pc = offset + addr;
	state->pc = addr;

}

void returnToCaller(State8085 *state)
{
	state->pc = (state->memory[state->sp] | (state->memory[state->sp + 1] << 8));
	state->sp += 2;
}


void run_program(int loc)
{
	state->pc = loc;
	
	while(state->pc != 66535)
	{
		(state->pc)++;
		uint8_t opcode[3];
		opcode[1] = state->memory[state->pc];
		opcode[2] = state->memory[state->pc + 1];
		switch (state->memory[state->pc - 1])
		{
			case 0x00:
				break; //NOP
			case 0x01: //LXI	B,word
				state->c = opcode[1];
				state->b = opcode[2];
				state->pc += 2;
				break;
			case 0x02: //STAX B
				state->memory[(state->b << 8) | state->c] = state->a;
				break;
			case 0x03: //INX B
				state->c++;
				if (state->c == 0)
					state->b++;
				break;
			case 0x04: //INR B
				state->b = addByte(state, state->b, 1, PRESERVE_CARRY);
				break;
			case 0x05: //DCR B
				state->b = subtractByte(state, state->b, 1, PRESERVE_CARRY);
				break;
			case 0x06: // MVI B, byte
				state->b = opcode[1];
				state->pc++;
				break;
			case 0x07: //RLC
			{
				uint8_t x = state->a;
				state->a = ((x & 0x80) >> 7) | (x << 1);
				state->cc.cy = (1 == ((x & 0x80) >> 7));
			}
			break;
			case 0x08:
				InvalidInstruction(state);
				break;
			case 0x09: // DAD B
			{
				uint32_t hl = (state->h << 8) | state->l;
				uint32_t bc = (state->b << 8) | state->c;
				uint32_t res = hl + bc;
				state->h = (res & 0xff00) >> 8;
				state->l = res & 0xff;
				state->cc.cy = ((res & 0xffff0000) > 0);
			}
			break;
			case 0x0a: //LDAX B
			{
				uint16_t offset = (state->b << 8) | state->c;
				state->a = state->memory[offset];
			}
			break;
			case 0x0b: //DCX B
				state->c--;
				if (state->c == 0xFF)
					state->b--;
				break;
			case 0x0c: //INR C
			{
				state->c = addByte(state, state->c, 1, PRESERVE_CARRY);
			}
			break;
			case 0x0d: //DCR    C
				state->c = subtractByte(state, state->c, 1, PRESERVE_CARRY);
				break;
			case 0x0e: // MVI C, byte
				state->c = opcode[1];
				state->pc++;
				break;
			case 0x0f: //RRC
			{
				uint8_t x = state->a;
				state->a = ((x & 1) << 7) | (x >> 1);
				state->cc.cy = (1 == (x & 1));
			}
			break;
			case 0x10:
				InvalidInstruction(state);
				break;
			case 0x11: //LXI	D,word
				state->e = opcode[1];
				state->d = opcode[2];
				state->pc += 2;
				break;
			case 0x12:  // STAX D
				state->memory[(state->d << 8) + state->e] = state->a;
				break;
			case 0x13: //INX    D
				state->e++;
				if (state->e == 0)
					state->d++;
				break;
			case 0x14: //INR D
				state->d = addByte(state, state->d, 1, PRESERVE_CARRY);
				break;
			case 0x15: //DCR D
				state->d = subtractByte(state, state->d, 1, PRESERVE_CARRY);
				break;
			case 0x16: // MVI D, byte
				state->d = opcode[1];
				state->pc++;
				break;
			case 0x17: // RAL
			{
				uint8_t x = state->a;
				state->a = state->cc.cy | (x << 1);
				state->cc.cy = (1 == ((x & 0x80) >> 7));
			}
			break;
			case 0x18:
				InvalidInstruction(state);
				break;
			case 0x19: //DAD D
			{
				uint32_t hl = (state->h << 8) | state->l;
				uint32_t de = (state->d << 8) | state->e;
				uint32_t res = hl + de;
				state->h = (res & 0xff00) >> 8;
				state->l = res & 0xff;
				state->cc.cy = ((res & 0xffff0000) != 0);
			}
			break;
			case 0x1a: //LDAX D
			{
				uint16_t offset = (state->d << 8) | state->e;
				state->a = state->memory[offset];
			}
			break;
			case 0x1b: //DCX D
				state->e--;
				if (state->e == 0xFF)
					state->d--;
				break;
			case 0x1c: //INR E
				state->e = addByte(state, state->e, 1, PRESERVE_CARRY);
				break;
			case 0x1d: //DCR E
				state->e = subtractByte(state, state->e, 1, PRESERVE_CARRY);
				break;
			case 0x1e: //MVI E, byte
				state->e = opcode[1];
				state->pc++;
				break;
			case 0x1f: // RAR
			{
				uint8_t x = state->a;
				state->a = (x >> 1) | (state->cc.cy << 7); /* From a number with higest bit as carry value */
				state->cc.cy = (1 == (x & 1));
			}
			break;
			case 0x20:
				UnimplementedInstruction(state);
				break; //RIM
			case 0x21: //LXI	H,word
				state->l = opcode[1];
				state->h = opcode[2];
				state->pc += 2;
				break;
			case 0x22: //SHLD word
			{
				uint16_t offset = (opcode[2] << 8) | opcode[1];
				state->memory[offset] = state->l;
				state->memory[offset + 1] = state->h;
				state->pc += 2;
			}
			break;
			case 0x23: //INX H
				state->l++;
				if (state->l == 0)
					state->h++;
				break;
			case 0x24: //INR H
				state->h = addByte(state, state->h, 1, PRESERVE_CARRY);
				break;
			break;
			case 0x25: //DCR H
				state->h = subtractByte(state, state->h, 1, PRESERVE_CARRY);
				break;
			case 0x26: //MVI H, byte
				state->h = opcode[1];
				state->pc++;
				break;
			case 0x27: // DAA
			{
				uint16_t res = state->a;
		
				if (state->cc.ac == 1 || (state->a & 0x0f) > 9)
					res = state->a + 6;
		
				ArithFlagsA(state, res, PRESERVE_CARRY);
				if ((uint8_t)res > 0xf)
					state->cc.ac = 1;
				state->a = (uint8_t)res;
		
				if (state->cc.cy == 1 || ((state->a >> 4) & 0x0f) > 9)
					res = state->a + 96;
		
				ArithFlagsA(state, res, UPDATE_CARRY);
				state->a = (uint8_t)res;
			}
			break;
			case 0x28:
				InvalidInstruction(state);
				break;
			case 0x29: // DAD H
			{
				uint32_t hl = (state->h << 8) | state->l;
				uint32_t res = hl + hl;
				state->h = (res & 0xff00) >> 8;
				state->l = res & 0xff;
				state->cc.cy = ((res & 0xffff0000) != 0);
			}
			break;
			case 0x2a: // LHLD Addr
			{
				uint16_t offset = (opcode[2] << 8) | (opcode[1]);
				uint8_t l = state->memory[offset];
				uint8_t h = state->memory[offset + 1];
				uint16_t v = h << 8 | l;
				state->h = v >> 8 & 0xFF;
				state->l = v & 0xFF;
				state->pc += 2;
			}
			break;
			case 0x2b: //DCX H
				state->l--;
				if (state->l == 0xFF)
					state->h--;
				break;
			case 0x2c: //INR L
				state->l = addByte(state, state->l, 1, PRESERVE_CARRY);
				break;
			break;
			case 0x2d: //DCR L
				state->l = subtractByte(state, state->l, 1, PRESERVE_CARRY);
				break;
			case 0x2e: // MVI L,byte
				state->l = opcode[1];
				state->pc++;
				break;
			case 0x2f: // CMA
				state->a ^= 0xFF;
				break;
			case 0x30:
				UnimplementedInstruction(state);
				break; // RIM
			case 0x31: // LXI SP, word
				state->sp = (opcode[2] << 8) | opcode[1];
				state->pc += 2;
				break;
			case 0x32: // STA word
			{
				uint16_t offset = (opcode[2] << 8) | (opcode[1]);
				state->memory[offset] = state->a;
				state->pc += 2;
			}
			break;
			case 0x33: // INX SP
				state->sp++;
				break;
			case 0x34: // INR M
			{
				uint16_t offset = (state->h << 8) | state->l;
				state->memory[offset] = addByte(state, state->memory[offset], 1, PRESERVE_CARRY);
			}
			break;
			case 0x35: // DCR M
			{
				uint16_t offset = (state->h << 8) | state->l;
				state->memory[offset] = subtractByte(state, state->memory[offset], 1, PRESERVE_CARRY);
			}
			break;
			case 0x36: // MVI M, byte
			{
				//AC set if lower nibble of h was zero prior to dec
				uint16_t offset = (state->h << 8) | state->l;
				state->memory[offset] = opcode[1];
				state->pc++;
			}
			break;
			case 0x37:
				state->cc.cy = 1;
				break; // STC
			case 0x38:
				InvalidInstruction(state);
				break;
			case 0x39: // DAD SP
			{
				uint16_t hl = (state->h << 8) | state->l;
				uint16_t sp = state->sp;
				uint32_t res = hl + sp;
				state->h = (res & 0xff00) >> 8;
				state->l = res & 0xff;
				state->cc.cy = ((res & 0xffff0000) > 0);
			}
			break;
				break;
			case 0x3a: // LDA word
			{
				uint16_t offset = (opcode[2] << 8) | (opcode[1]);
				state->a = state->memory[offset];
				state->pc += 2;
			}
			break;
			case 0x3b: // DCX SP
				state->sp--;
				break;
			case 0x3c: // INR A
				state->a = addByte(state, state->a, 1, PRESERVE_CARRY);
				break;
			case 0x3d: // DCR A
				state->a = subtractByte(state, state->a, 1, PRESERVE_CARRY);
				break;
			case 0x3e: // MVI A, byte
				state->a = opcode[1];
				state->pc++;
				break;
			case 0x3f: // CMC
				if (0 == state->cc.cy)
					state->cc.cy = 1;
				else
					state->cc.cy = 0;
				break;
			case 0x40:
				state->b = state->b;
				break; // MOV B, B
			case 0x41:
				state->b = state->c;
				break; // MOV B, C
			case 0x42:
				state->b = state->d;
				break; // MOV B, D
			case 0x43:
				state->b = state->e;
				break; // MOV B, E
			case 0x44:
				state->b = state->h;
				break; // MOV B, H
			case 0x45:
				state->b = state->l;
				break; // MOV B, L
			case 0x46: // MOV B, M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->b = state->memory[offset];
			}
			break;
			case 0x47:
				state->b = state->a;
				break; // MOV B, A
			case 0x48:
				state->c = state->b;
				break; // MOV C, B
			case 0x49:
				state->c = state->c;
				break; // MOV C, C
			case 0x4a:
				state->c = state->d;
				break; // MOV C, D
			case 0x4b:
				state->c = state->e;
				break; // MOV C, E
			case 0x4c:
				state->c = state->h;
				break; // MOV C, H
			case 0x4d:
				state->c = state->l;
				break; // MOV C, L
			case 0x4e: // MOV C, M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->c = state->memory[offset];
			}
			break;
			case 0x4f:
				state->c = state->a;
				break; // MOV C, A
			case 0x50:
				state->d = state->b;
				break; // MOV D, B
			case 0x51:
				state->d = state->c;
				break; // MOV D, B
			case 0x52:
				state->d = state->d;
				break; // MOV D, B
			case 0x53:
				state->d = state->e;
				break; // MOV D, B
			case 0x54:
				state->d = state->h;
				break; // MOV D, B
			case 0x55:
				state->d = state->l;
				break; // MOV D, B
			case 0x56: // MOV D, M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->d = state->memory[offset];
			}
			break;
			case 0x57:
				state->d = state->a;
				break; // MOV D, A
			case 0x58:
				state->e = state->b;
				break; // MOV E, B
			case 0x59:
				state->e = state->c;
				break; // MOV E, C
			case 0x5a:
				state->e = state->d;
				break; // MOV E, D
			case 0x5b:
				state->e = state->e;
				break; // MOV E, E
			case 0x5c:
				state->e = state->h;
				break; // MOV E, H
			case 0x5d:
				state->e = state->l;
				break; // MOV E, L
			case 0x5e: // MOV E, M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->e = state->memory[offset];
			}
			break;
			case 0x5f:
				state->e = state->a;
				break; // MOV E, A
			case 0x60:
				state->h = state->b;
				break; // MOV H, B
			case 0x61:
				state->h = state->c;
				break; // MOV H, C
			case 0x62:
				state->h = state->d;
				break; // MOV H, D
			case 0x63:
				state->h = state->e;
				break; // MOV H, E
			case 0x64:
				state->h = state->h;
				break; // MOV H, H
			case 0x65:
				state->h = state->l;
				break; // MOV H, L
			case 0x66: // MOV H, M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->h = state->memory[offset];
			}
			break;
			case 0x67:
				state->h = state->a;
				break; // MOV H, A
			case 0x68:
				state->l = state->b;
				break; // MOV L, B
			case 0x69:
				state->l = state->c;
				break; // MOV L, C
			case 0x6a:
				state->l = state->d;
				break; // MOV L, D
			case 0x6b:
				state->l = state->e;
				break; // MOV L, E
			case 0x6c:
				state->l = state->h;
				break; // MOV L, H
			case 0x6d:
				state->l = state->l;
				break; // MOV L, L
			case 0x6e: // MOV L, M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->l = state->memory[offset];
			}
			break;
			case 0x6f:
				state->l = state->a;
				break; // MOV L, A
			case 0x70: // MOV M, B
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->b;
			}
			break;
			case 0x71: // MOV M, C
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->c;
			}
			break;
			case 0x72: // MOV M, D
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->d;
			}
			break;
			case 0x73: // MOV M, E
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->e;
			}
			break;
			case 0x74: // MOV M, H
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->h;
			}
			break;
			case 0x75: // MOV M, L
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->l;
			}
			break;
			case 0x76:
				return;
				break; // HLT
			case 0x77: // MOV M, A
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->a;
			}
			break;
			case 0x78:
				state->a = state->b;
				break; // MOV A, B
			case 0x79:
				state->a = state->c;
				break; // MOV A, C
			case 0x7a:
				state->a = state->d;
				break; // MOV A, D
			case 0x7b:
				state->a = state->e;
				break; // MOV A, E
			case 0x7c:
				state->a = state->h;
				break; // MOV A, H
			case 0x7d:
				state->a = state->l;
				break; // MOV A, L
			case 0x7e: // MOV A, M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->a = state->memory[offset];
			}
			break;
			case 0x7f:
				state->a = state->a;
				break; // MOV A, A
			case 0x80: // ADD B
				state->a = addByte(state, state->a, state->b, UPDATE_CARRY);
				break;
			case 0x81: // ADD C
				state->a = addByte(state, state->a, state->c, UPDATE_CARRY);
				break;
			case 0x82: // ADD D
				state->a = addByte(state, state->a, state->d, UPDATE_CARRY);
				break;
			case 0x83: // ADD E
				state->a = addByte(state, state->a, state->e, UPDATE_CARRY);
				break;
			case 0x84: // ADD H
				state->a = addByte(state, state->a, state->h, UPDATE_CARRY);
				break;
			case 0x85: // ADD L
				state->a = addByte(state, state->a, state->l, UPDATE_CARRY);
				break;
			case 0x86: // ADD M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->a = addByte(state, state->a, state->memory[offset], UPDATE_CARRY);
			}
			break;
			case 0x87: // ADD A
				state->a = addByte(state, state->a, state->a, UPDATE_CARRY);
				break;
			case 0x88: // ADC B
				state->a = addByteWithCarry(state, state->a, state->b, UPDATE_CARRY);
				break;
			case 0x89: // ADC C
				state->a = addByteWithCarry(state, state->a, state->c, UPDATE_CARRY);
				break;
			break;
			case 0x8a: // ADC D
				state->a = addByteWithCarry(state, state->a, state->d, UPDATE_CARRY);
				break;
			case 0x8b: // ADC E
				state->a = addByteWithCarry(state, state->a, state->e, UPDATE_CARRY);
				break;
			case 0x8c: // ADC H
				state->a = addByteWithCarry(state, state->a, state->h, UPDATE_CARRY);
				break;
			case 0x8d: // ADC L
				state->a = addByteWithCarry(state, state->a, state->l, UPDATE_CARRY);
				break;
			case 0x8e: // ADC M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->a = addByteWithCarry(state, state->a, state->memory[offset], UPDATE_CARRY);
			}
			break;
			case 0x8f: // ADC A
				state->a = addByteWithCarry(state, state->a, state->a, UPDATE_CARRY);
				break;
			case 0x90: // SUB B
				state->a = subtractByte(state, state->a, state->b, UPDATE_CARRY);
				break;
			case 0x91: // SUB C
				state->a = subtractByte(state, state->a, state->c, UPDATE_CARRY);
				break;
			case 0x92: // SUB D
				state->a = subtractByte(state, state->a, state->d, UPDATE_CARRY);
				break;
			case 0x93: // SUB E
				state->a = subtractByte(state, state->a, state->e, UPDATE_CARRY);
				break;
			case 0x94: // SUB H
				state->a = subtractByte(state, state->a, state->h, UPDATE_CARRY);
				break;
			case 0x95: // SUB L
				state->a = subtractByte(state, state->a, state->l, UPDATE_CARRY);
				break;
			case 0x96: // SUB M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->a = subtractByte(state, state->a, state->memory[offset], UPDATE_CARRY);
			}
			break;
			case 0x97: // SUB A
				state->a = subtractByte(state, state->a, state->a, UPDATE_CARRY);
				break;
			case 0x98: // SBB B
				state->a = subtractByteWithBorrow(state, state->a, state->b, UPDATE_CARRY);
				break;
			case 0x99: // SBB C
				state->a = subtractByteWithBorrow(state, state->a, state->c, UPDATE_CARRY);
				break;
			case 0x9a: // SBB D
				state->a = subtractByteWithBorrow(state, state->a, state->d, UPDATE_CARRY);
				break;
			case 0x9b: // SBB E
				state->a = subtractByteWithBorrow(state, state->a, state->e, UPDATE_CARRY);
				break;
			case 0x9c: // SBB H
				state->a = subtractByteWithBorrow(state, state->a, state->h, UPDATE_CARRY);
				break;
			case 0x9d: // SBB L
				state->a = subtractByteWithBorrow(state, state->a, state->l, UPDATE_CARRY);
				break;
			case 0x9e: // SBB M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->a = subtractByteWithBorrow(state, state->a, state->memory[offset], UPDATE_CARRY);
			}
			break;
			case 0x9f: // SBB A
				state->a = subtractByteWithBorrow(state, state->a, state->a, UPDATE_CARRY);
				break;
			case 0xa0: // ANA B
				state->a = state->a & state->b;
				LogicFlagsA(state, 1);
				break;
			case 0xa1: // ANA C
				state->a = state->a & state->c;
				LogicFlagsA(state, 1);
				break;
			case 0xa2: // ANA D
				state->a = state->a & state->d;
				LogicFlagsA(state, 1);
				break;
			case 0xa3: // ANA E
				state->a = state->a & state->e;
				LogicFlagsA(state, 1);
				break;
			case 0xa4: // ANA H
				state->a = state->a & state->h;
				LogicFlagsA(state, 1);
				break;
			case 0xa5: // ANA L
				state->a = state->a & state->l;
				LogicFlagsA(state, 1);
				break;
			case 0xa6: // ANA M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->a = state->a & state->memory[offset];
				LogicFlagsA(state, 1);
			}
			break;
			case 0xa7: // ANA A
				state->a = state->a & state->a;
				LogicFlagsA(state, 1);
				break;
			case 0xa8:
				state->a = state->a ^ state->b;
				LogicFlagsA(state, 0);
				break; // XRA B
			case 0xa9:
				state->a = state->a ^ state->c;
				LogicFlagsA(state, 0);
				break; // XRA C
			case 0xaa:
				state->a = state->a ^ state->d;
				LogicFlagsA(state, 0);
				break; // XRA D
			case 0xab:
				state->a = state->a ^ state->e;
				LogicFlagsA(state, 0);
				break; // XRA E
			case 0xac:
				state->a = state->a ^ state->h;
				LogicFlagsA(state, 0);
				break; // XRA H
			case 0xad:
				state->a = state->a ^ state->l;
				LogicFlagsA(state, 0);
				break; // XRA L
			case 0xae: // XRA M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->a = state->a ^ state->memory[offset];
				LogicFlagsA(state, 0);
			}
			break;
			case 0xaf:
				state->a = state->a ^ state->a;
				LogicFlagsA(state, 0);
				break; // XRA A
			case 0xb0:
				state->a = state->a | state->b;
				LogicFlagsA(state, 0);
				break; // ORA B
			case 0xb1:
				state->a = state->a | state->c;
				LogicFlagsA(state, 0);
				break; // ORA C
			case 0xb2:
				state->a = state->a | state->d;
				LogicFlagsA(state, 0);
				break; // ORA D
			case 0xb3:
				state->a = state->a | state->e;
				LogicFlagsA(state, 0);
				break; // ORA E
			case 0xb4:
				state->a = state->a | state->h;
				LogicFlagsA(state, 0);
				break; // ORA H
			case 0xb5:
				state->a = state->a | state->l;
				LogicFlagsA(state, 0);
				break; // ORA L
			case 0xb6: // ORA M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				state->a = state->a | state->memory[offset];
				LogicFlagsA(state, 0);
			}
			break;
			case 0xb7:
				state->a = state->a | state->a;
				LogicFlagsA(state, 0);
				break; // ORA A
			case 0xb8: // CMP B
				subtractByte(state, state->a, state->b, UPDATE_CARRY);
				break;
			case 0xb9: // CMP C
				subtractByte(state, state->a, state->c, UPDATE_CARRY);
				break;
			case 0xba: // CMP D
				subtractByte(state, state->a, state->d, UPDATE_CARRY);
				break;
			case 0xbb: // CMP E
				subtractByte(state, state->a, state->e, UPDATE_CARRY);
				break;
			case 0xbc: // CMP H
				subtractByte(state, state->a, state->h, UPDATE_CARRY);
				break;
			case 0xbd: // CMP L
				subtractByte(state, state->a, state->l, UPDATE_CARRY);
				break;
			case 0xbe: // CMP M
			{
				uint16_t offset = (state->h << 8) | (state->l);
				subtractByte(state, state->a, state->memory[offset], UPDATE_CARRY);
			}
			break;
			case 0xbf: // CMP A
				subtractByte(state, state->a, state->a, UPDATE_CARRY);
				break;
			case 0xc0: // RNZ
				if (0 == state->cc.z)
					returnToCaller(state);
				break;
			case 0xc1: // POP B
			{
				state->c = state->memory[state->sp];
				state->b = state->memory[state->sp + 1];
				state->sp += 2;
			}
			break;
			case 0xc2: // JNZ Addr
				if (0 == state->cc.z){
					state->pc = ((opcode[2] << 8) | opcode[1]);
				}
				else
					state->pc += 2;
				break;
			case 0xc3: // JMP Addr
				state->pc = ((opcode[2] << 8) | opcode[1]);
				state->pc += 2;
				break;
			case 0xc4: // CNZ Addr
				if (0 == state->cc.z)
				{
					uint16_t pc = state->pc;
					state->memory[state->sp - 1] = (pc >> 8) & 0xff;
					state->memory[state->sp - 2] = (pc & 0xff);
					state->sp = state->sp - 2;
					state->pc = (opcode[2] << 8) | opcode[1];
				}
				else
					state->pc += 2;
				break;
			case 0xc5: // PUSH   B
			{
				state->memory[state->sp - 1] = state->b;
				state->memory[state->sp - 2] = state->c;
				state->sp = state->sp - 2;
			}
			break;
			case 0xc6: // ADI byte
			{
				uint16_t x = (uint16_t)state->a + (uint16_t)opcode[1];
				state->cc.z = ((x & 0xff) == 0);
				state->cc.s = (0x80 == (x & 0x80));
				state->cc.p = parity((x & 0xff), 8);
				state->cc.cy = (x > 0xff);
				state->a = (uint8_t)x;
				state->pc++;
			}
			break;
			case 0xc7: // RST 0
				return; //Unimplemented Instruction
				break;
			case 0xc8: // RZ
				if (1 == state->cc.z)
					returnToCaller(state);
				break;
			case 0xc9: // RET
				returnToCaller(state);
				break;
			case 0xca: // JZ Addr
				if (1 == state->cc.z)
				{
					state->pc = ((opcode[2] << 8) | opcode[1]);
					state->pc += 2;
				}
				else
					state->pc += 2;
				break;
			case 0xcb:
				InvalidInstruction(state);
				break;
			case 0xcc: // CZ Addr
				if (1 == state->cc.z)
				{
					call(state, (opcode[2] << 8) | opcode[1]);
					
				}
				else
					state->pc += 2;
				break;
			case 0xcd: // CALL Addr

				call(state, (opcode[2] << 8) | opcode[1]);
				
				break;
			case 0xce: // ACI d8
				state->a = addByteWithCarry(state, state->a, opcode[1], UPDATE_CARRY);
				state->pc++;
				break;
			case 0xcf: // RST 1
				return; //Unimplemented Instruction
				break;
			case 0xd0: // RNC
				if (0 == state->cc.cy)
					returnToCaller(state);
				break;
			case 0xd1: // POP D
			{
				state->e = state->memory[state->sp];
				state->d = state->memory[state->sp + 1];
				state->sp += 2;
			}
			break;
			case 0xd2: // JNC Addr
				if (0 == state->cc.cy)
				{
					state->pc = ((opcode[2] << 8) | opcode[1]);
					state->pc += 2;
				}
				else
					state->pc += 2;
				break;
			case 0xd3:
				UnimplementedInstruction(state);
				state->pc++;
				break;
			case 0xd4: // CNC Addr
				if (0 == state->cc.cy)
				{
					call(state, (opcode[2] << 8) | opcode[1]);
					
				}
				else
					state->pc += 2;
				break;
			case 0xd5: //PUSH   D
			{
				state->memory[state->sp - 1] = state->d;
				state->memory[state->sp - 2] = state->e;
				state->sp = state->sp - 2;
			}
			break;
			case 0xd6: // SUI d8
				state->a = subtractByte(state, state->a, opcode[1], UPDATE_CARRY);
				state->pc++;
				break;
			case 0xd7: // RST 2
				return; //Unimplemented Instruction
				break;
			case 0xd8: // RC
				if (1 == state->cc.cy)
					returnToCaller(state);
				break;
			case 0xd9:
				InvalidInstruction(state);
				break;
			case 0xda: // JC Addr
				if (1 == state->cc.cy)
				{
					state->pc = ((opcode[2] << 8) | opcode[1]);
				}
				else
					state->pc += 2;
				break;
			case 0xdb:
				UnimplementedInstruction(state);
				break; // IN d8
			case 0xdc: // CC Addr
				if (1 == state->cc.cy)
				{
					call(state, (opcode[2] << 8) | opcode[1]);
					
				}
				else
					state->pc += 2;
				break;
			case 0xdd:
				InvalidInstruction(state);
				break;
			case 0xde: // SBI d8
				state->a = subtractByteWithBorrow(state, state->a, opcode[1], UPDATE_CARRY);
				state->pc++;
				break;
			case 0xdf: // RST 3
				return; //Unimplemented Instruction
				break;
			case 0xe0: // RPO
				if (0 == state->cc.cy)
					returnToCaller(state);
				break;
			case 0xe1: // POP H
			{
				state->l = state->memory[state->sp];
				state->h = state->memory[state->sp + 1];
				state->sp += 2;
			}
			break;
			case 0xe2: // JPO Addr
				if (0 == state->cc.p)
				{
					state->pc = ((opcode[2] << 8) | opcode[1]);
				}
				else
					state->pc += 2;
				break;
			case 0xe3: // XTHL
			{
				uint16_t spL = state->memory[state->sp];
				uint16_t spH = state->memory[state->sp + 1];
				state->memory[state->sp] = state->l;
				state->memory[state->sp + 1] = state->h;
				state->h = spH;
				state->l = spL;
			}
			break;
			case 0xe4: // CPO Addr
				if (0 == state->cc.p)
					call(state, (opcode[2] << 8) | opcode[1]);
				else
					state->pc += 2;
				break;
			case 0xe5: // PUSH H
			{
				state->memory[state->sp - 1] = state->h;
				state->memory[state->sp - 2] = state->l;
				state->sp = state->sp - 2;
			}
			break;
			case 0xe6: // ANI byte
			{
				state->a = state->a & opcode[1];
				LogicFlagsA(state, 1);
				state->pc++;
			}
			break;
			case 0xe7: // RST 4
				return; //Unimplemented Instruction
				break;
			case 0xe8: // RPE
				if (0 == state->cc.cy)
					returnToCaller(state);
				break;
			case 0xe9: // PCHL
				state->pc = (state->h << 8) | state->l;
				break;
			case 0xea: // JPE Addr
				if (1 == state->cc.p)
				{
					state->pc =  ((opcode[2] << 8) | opcode[1]);
				}
				else
					state->pc += 2;
				break;
			case 0xeb: // XCHG
			{
				uint8_t save1 = state->d;
				uint8_t save2 = state->e;
				state->d = state->h;
				state->e = state->l;
				state->h = save1;
				state->l = save2;
			}
			break;
			case 0xec: // CPE Addr
				if (1 == state->cc.p)
					call(state, (opcode[2] << 8) | opcode[1]);
				else
					state->pc += 2;
				break;
			case 0xed:
				InvalidInstruction(state);
				break;
			case 0xee: // XRI d8
				state->a = state->a ^ opcode[1];
				LogicFlagsA(state, 0);
				state->pc++;
				break;
			case 0xef: // RST 5
				return; //Unimplemented Instruction
				break;
			case 0xf0: // RP
				if (0 == state->cc.s)
					returnToCaller(state);
				break;
			case 0xf1: //POP PSW
			{
				state->a = state->memory[state->sp + 1];
				uint8_t psw = state->memory[state->sp];
				state->cc.z = (0x01 == (psw & 0x01));
				state->cc.s = (0x02 == (psw & 0x02));
				state->cc.p = (0x04 == (psw & 0x04));
				state->cc.cy = (0x05 == (psw & 0x08));
				state->cc.ac = (0x10 == (psw & 0x10));
				state->sp += 2;
			}
			break;
			case 0xf2: // JP Addr
				if (0 == state->cc.s)
				{
					state->pc = ((opcode[2] << 8) | opcode[1]);
				}
				else
					state->pc += 2;
				break;
			case 0xf3: // DI
				state->int_enable = 0;
				break;
			case 0xf4: // CP Addr
				if (0 == state->cc.s)
				{
					call(state, (opcode[2] << 8) | opcode[1]);
					
				}
				else
					state->pc += 2;
				break;
			case 0xf5: // PUSH PSW
			{
				state->memory[state->sp - 1] = state->a;
				uint8_t psw = (state->cc.z |
							   state->cc.s << 1 |
							   state->cc.p << 2 |
							   state->cc.cy << 3 |
							   state->cc.ac << 4);
				state->memory[state->sp - 2] = psw;
				state->sp = state->sp - 2;
			}
			break;
			case 0xf6: // ORI d8
				state->a = state->a | opcode[1];
				LogicFlagsA(state, 0);
				state->pc++;
				break;
			case 0xf7: // RST 6
				return; //Unimplemented Instruction
				break;
			case 0xf8: // RM
				if (1 == state->cc.s)
					returnToCaller(state);
				break;
			case 0xf9: // SPHL
				state->sp = (state->h << 8) | state->l;
				break;
			case 0xfa: // JM Addr
				if (1 == state->cc.s)
				{
					state->pc =  ((opcode[2] << 8) | opcode[1]);
					
				}
				else
					state->pc += 2;
				break;
		
			case 0xfb:
				state->int_enable = 1;
				break; // EI
			case 0xfc: // CM Addr
				if (1 == state->cc.s)
				{
					call(state, (opcode[2] << 8) | opcode[1]);
				}
				else
					state->pc += 2;
				break;
			case 0xfd:
				InvalidInstruction(state);
			case 0xfe: // CPI d8
			{
				uint8_t x = state->a - opcode[1];
				state->cc.z = (x == 0);
				state->cc.s = (0x80 == (x & 0x80));
				state->cc.p = parity(x, 8);
				state->cc.cy = (state->a < opcode[1]);
				state->pc++;
			}
			break;
			case 0xff: // RST 7
				return; //Unimplemented Instruction
				break;
		}
		
		
	}
	
}
	
	
