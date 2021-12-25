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

	/* Creating the opcode hashtable */
	get_optable(opt);
	/* Creating the symbol hashtable */
	init_symtab();

	/* Initialising the location counter to 0 */
	int locctr = 0, line_no = 1;

	/* Loop to read instructions from the input file */
	while(!feof(inp))
	{
		char *line, *label;

		/* Reading line from the input file */
		line = read_line(inp); 

		if(feof(inp))
			break;

		/* Check whether a label exists in the line */
		if(label = check_label(line)) 
		{
			/* Insert the label in the symbol table */
			if(!insert_label(label, locctr, sym))
				printf("ERROR: Line %d: Label already exists!!\n", line_no);
		}
	
		/* Extract the mnemonic from the line */	
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
