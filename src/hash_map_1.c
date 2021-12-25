#include "../inc/hash_map_1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

long long int string_to_int(char* input)
{
	int i =0;
	long long int result = 0;
	int r = 0;
	//printf("key is : %s\n",input);
	while(input[i]!='\0')
	{
		int temp = input[i];
		result += temp*pow(10,r);
		r++;
		i++;	
	}
	return result;
}

static struct opdata *array;
static int capacity = 19997;

int hashCode(char* key)
{
	long long int result = string_to_int(key);
	return (result % capacity);
}


void init_map()
{
	int i;
	array = (struct opdata*) malloc(capacity * sizeof(struct opdata));
	for (i = 0; i < capacity; i++) 
    {
    	array[i].opcode = NULL;
		array[i].length = 0;
	}
}

void insert(char* key, struct opdata* opd)
{
	int index = hashCode(key);
	//printf("INDEX: %d\n", index);
	if(array[index].length == 0)
	{
		array[index].length = opd->length;
		array[index].opcode = (char*) malloc(3 * sizeof(char));
		strcpy(array[index].opcode, opd->opcode);
	}
}

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