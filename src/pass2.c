#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../inc/utils.h"
#include "../inc/hash_map_1.h"
#include "../inc/hash_map_2.h"
#include "../inc/pass2.h"

void pass2(char* input_file)
{
	int success = 1, loc = 0;
	char objcode[10000][3];
	/* Opening the required files */
	FILE* opt = fopen("../data/optable.txt", "r");
	FILE* sym = fopen("../data/symtable.txt", "r");
	FILE* inp = fopen(input_file, "r");
	FILE* out = fopen("../output/output.txt", "w");
	FILE* lbl = fopen("../data/instructions_with_labels.txt", "r");

	/* Getting the opcode hashtable */
	get_optable(opt);
	/* Getting the symbol table */
	get_symtable(sym);

	int line_no = 1;
	//read the instructions line by line
	while(!feof(inp))
	{
		char* line = read_line(inp);
		
		if(feof(inp)) break;
		/* Remove label if any */
		check_label(line);
		/* Extract the mnemonic from the line */
		char* mnemonic = get_mnemonic(line);
		struct opdata* s = get(mnemonic);
		strcpy(objcode[loc], s->opcode);
		loc++;
		
		if(mnemonic == NULL)
		{
			success = 0;
			printf("ERROR: Line %d: Invalid opcode!!\n", line_no);
		}
		else
		{
			char* operand = get_operand(line, mnemonic);
			if(has_label(mnemonic, lbl))
			{
				//get the address of the symbol from symtab
				char* op = get_value(operand);
				//operand is the 2 byte address
				strcpy(operand, op); 
			}
			//check if operand length and operand(s) are valid
			if(is_valid_operand(operand, s->length - 1))
			{
				char** operands = get_operands(operand, s->length - 1);
				for(int i = 0 ; i < s->length - 1; i++)
				{
					strcpy(objcode[loc], operands[i]);
					loc++;
				}
			}
			else	//failure in assembly
			{
				success = 0;
				printf("ERROR: Line %d: Invalid operands!!\n", line_no);
			}
			

		}

		line_no++;
	}

	if(success == 1)	//if no error found 
	{
		for(int i = 0; i < loc; i++)
		{
			//write to output.txt the object codes in hex
			fprintf(out, "%s\n", objcode[i]);
		}
		printf("Object code generated...stored in output.txt\n");
	}
	//close all the files
	fclose(opt);
	fclose(sym);
	fclose(inp);
	fclose(out);
}