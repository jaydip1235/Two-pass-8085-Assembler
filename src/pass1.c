#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../inc/utils.h"
#include "../inc/hash_map_1.h"
#include "../inc/hash_map_2.h"
#include "../inc/pass1.h"

void pass1(char* input_file)
{
	/* Opening the required files */
	FILE* opt = fopen("../data/optable.txt", "r");
	FILE* sym = fopen("../data/symtable.txt", "w");
	FILE* inp = fopen(input_file, "r");

	get_optable(opt);	//Creating the hashtable for storing opcodes
	init_symtab();			/* Create an empty symtab */

	/* Initialising the location counter to 0 */
	int locctr = 0, line_no = 1;

	/* Read instructions from the input file line by line*/
	while(!feof(inp))
	{
		char *line, *label;
		line = read_line(inp); 

		if(feof(inp)) 	break;

		/* Check if the line starts with label and whether the label is valid or not*/
		if(label = check_label(line)) 
		{
			/* Insert the label in the symbol table */
			if(!insert_label(label, locctr, sym))
				printf("ERROR: Line %d: Label already exists!!\n", line_no);
		}
	
		/* Extract the instruction mnemonic from line */	
		char* mnemonic = get_mnemonic(line);
		/* If mnemonic exists update the location counter */
		if(mnemonic == NULL)
		{
			printf("ERROR: Line %d: Invalid opcode!!\n", line_no);
		}
		else
		{
			locctr += get(mnemonic)->length;	
		}
		
		line_no++;
	}

	fclose(opt);
	fclose(sym);
	fclose(inp);
	
}
