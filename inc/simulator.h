#ifndef SIMULATOR_H
#define SIMULATOR_H

typedef struct
{
	uint8_t z : 1;
	uint8_t s : 1;
	uint8_t p : 1;
	uint8_t cy : 1;
	uint8_t ac : 1;
	uint8_t pad : 3; // 3 empty
} Flags;


typedef struct
{
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	uint16_t sp;
	uint16_t pc;
	Flags cc;
	uint8_t int_enable;
	uint8_t *memory;
} State8085;

State8085* state;

typedef enum 
{
	UPDATE_CARRY = 0, PRESERVE_CARRY = 1
} should_preserve_carry;

void init_state();
void enter_program(char*, int);
void enter_data(int, int);
void show_data();
void show_reg_flags();
void run_program(int);

#endif
