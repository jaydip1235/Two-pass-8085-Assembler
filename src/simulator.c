#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include "../inc/simulator.h"

// Function to initialise the registers, flags and memory 
void init_state()
{
	state = (State8085*)malloc(sizeof(State8085));
	printf("Initialing registers, flags and memory\n");
	state->a = 0; state->b = 0; state->c = 0; state->d = 0; state->e = 0; state->h = 0; state->l = 0; state->sp = 0; state->pc = 0;
	state->cc.z = 0; state->cc.s = 0; state->cc.p = 0; state->cc.cy = 0; state->cc.ac = 0; state->cc.pad = 0;
	
	state->memory = (uint8_t*)malloc(65536*sizeof(uint8_t));
	for(int i = 0; i < 4096; i++)
	{
		state->memory[i] = 0;
	}
}

void enter_program(char* filename, int start)
{
	int i = 0,num;
	FILE* fp = fopen(filename, "r");
	while(!feof(fp))
	{
		fscanf(fp, "%x", &num);
		state->memory[start + i] = num;
		i++;
	}
	fclose(fp);
}

void enter_data(int data,int loc)
{
	state->memory[loc]= data;
}

void show_data()
{
	for(int i = 0; i < 65536; i++)
	{
		if(i%5 == 0)
			printf("\n");
		printf("%04x  %02x\t", i, state->memory[i]);
		if((i + 1)%100 == 0)
		{
			char e;
			printf("\nPress enter to show more\n");
			scanf("%c", &e);
			if(e == 'q')
				return;
		}
	}
}

void show_reg_flags()
{
	printf("Register A: 0x%02x\n", state->a);
	printf("Register B: 0x%02x\n", state->a);
	printf("Register C: 0x%02x\n", state->a);
	printf("Register D: 0x%02x\n", state->a);
	printf("Register E: 0x%02x\n", state->a);
	printf("Register H: 0x%02x\n", state->a);
	printf("Register L: 0x%02x\n", state->a);
	printf("Program counter: 0x%04x\n", state->a);
	printf("Stack pointer: 0x%04x\n", state->a);
	printf("Flag Z: %d\n", state->cc.z);
	printf("Flag S: %d\n", state->cc.s);
	printf("Flag P: %d\n", state->cc.p);
	printf("Flag CY: %d\n", state->cc.cy);
	printf("Flag AC: %d\n", state->cc.ac);
	
}

int main()
{
	
	init_state();
	
	int ch, data, loc;
	 int start;
	char choice = 'y';
	while(choice == 'y' || choice == 'Y')
	{
		char filename[] = "../output/";
		char f[20];
		printf("Enter choice:\n");
		printf("1. Enter program into memory\n");
		printf("2. Enter data in memory\n");
		printf("3. Show memory\n");
		printf("4. Show registers and flags\n");
		printf("5. Run program\n");
		printf("6. Exit\n");
		
		scanf("%d",&ch);
		
		switch(ch)
		{
			case 1: 
					printf("Enter program file name:\n");
					scanf("%s", f);
					strcat(filename, f);
					printf("Enter starting memory location to store program:\n");
					scanf("%x", &start);
					enter_program(filename, start);
					break;
			case 2:
					printf("Enter data in hexadecimal:\n");
					scanf("%x", &data);
					printf("Enter memory location where data is to be inserted:\n");
					scanf("%x", &loc);
					enter_data(data, loc);
					break;
			case 3:
					show_data();
					break;
			case 4:
					show_reg_flags();
					break;
			case 5: 
					printf("Enter starting memory location of program: \n");
					scanf("%x", &loc);
					run_program(loc);
					break;
			case 6:
					exit(0);
			default:
					exit(0);
				
			printf("Want to enter more?(y/n)");
			scanf("%c", &choice);
		}
	}	
}
