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
	while(!feof(inp))
	{
		char* line = read_line(inp);
		
		if(feof(inp))
			break;

		/* Removing label from line if any */
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
				char* op = get_value(operand);
				strcpy(operand, op); 
			}

			if(is_valid_operand(operand, s->length - 1))
			{
				char** operands = get_operands(operand, s->length - 1);

				for(int i = 0 ; i < s->length - 1; i++)
				{
					strcpy(objcode[loc], operands[i]);
					loc++;
				}
			}
			else
			{
				success = 0;
				printf("ERROR: Line %d: Invalid operands!!\n", line_no);
			}
			

		}

		line_no++;
	}

	if(success == 1)
	{
		for(int i = 0; i < loc; i++)
		{
			printf("%s\n", objcode[i]);
			fprintf(out, "%s\n", objcode[i]);
		}
	}
	
	fclose(opt);
	fclose(sym);
	fclose(inp);
	fclose(out);
}