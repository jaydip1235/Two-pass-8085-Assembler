#include "../inc/hash_map_1.h"
#include "../inc/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static struct opdata *array;
static int capacity = 19997;

int hashCode(char* key)
{
	long long int result = string_to_int(key);
	return (result % capacity);
}

//initialize a hashtable for storing opcodes of mnemonics
void init_optab()
{
	int i;
	array = (struct opdata*) malloc(capacity * sizeof(struct opdata));
	for (i = 0; i < capacity; i++) 
    {
    	array[i].opcode = NULL;
		array[i].length = 0;
	}
}

//fp is the file containing the opcodes for mnemonics
//generates the optable
void get_optable(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	int sl;
	char mnemonic[10];
	char opcode[3];
	int length;

	init_optab();
	/* Reading line from file until EOF */
	while(fscanf(fp,"%d. %s %s %d",&sl,mnemonic,opcode,&length)!=EOF)
	{
		/* Inserting opcodes into hash table */
		struct opdata opd;
		opd.length = length;
		opd.opcode = (char*)malloc(sizeof(char) * 3);
		strcpy(opd.opcode, opcode);
		insert_in_optab(mnemonic, &opd);

	}
}

//given a struct opdata stores in the optable
void insert_in_optab(char* key, struct opdata* opd)
{
	int index = hashCode(key);
	if(array[index].length == 0)
	{
		array[index].length = opd->length;
		array[index].opcode = (char*) malloc(3 * sizeof(char));
		strcpy(array[index].opcode, opd->opcode);
	}
}

//given a mnemonic returns its opcode if present in the optab; 
struct opdata* get(char* key)
{
	int index = hashCode(key);
	struct opdata* temp = (struct opdata*) malloc(capacity * sizeof(struct opdata));;
	if(array[index].length > 0)
	{
		temp->length = array[index].length;
		temp->opcode = (char*) malloc(3 * sizeof(char));
		strcpy(temp->opcode, array[index].opcode);
		return temp;
	}
		
}