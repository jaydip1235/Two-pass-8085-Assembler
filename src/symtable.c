#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../inc/hash_map_2.h"
#include "../inc/utils.h"

static struct data *array;
static int capacity = 19997;

int hashedValue(char* key)
{
	long long int result = string_to_int(key);
	return (result % capacity);
}
//initialize the symtab with null values and flags as 0
void init_symtab()
{
	array = (struct data*) malloc(capacity * sizeof(struct data));
	int i =0;
	while(i < capacity)
	{
		array[i].key = NULL;
		array[i].value = NULL;
		array[i].flag = 0;
		i++;
	}
}
void get_symtable(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);

	init_symtab();
	char label[30];
	char address[5];
	/* Reading line from file until EOF */
	while(fscanf(fp,"%s %s",label, address)!=EOF)
	{
		/* Inserting each symbol into hash table */
		insert_in_symtab(label, address);
	}
}

//inserts a symbol : takes label name and label location as arguments

void insert_in_symtab(char* key, char* value)
{
	int index = hashedValue(key);
	array[index].key = (char*)malloc(30 * sizeof(char));
	array[index].value = (char*)malloc(5 * sizeof(char));
	strcpy(array[index].key, key);
	strcpy(array[index].value,value);
	array[index].flag = 1;
}
//given a label returns its address/value if present in the symtab; 
//else returns NULL
char* get_value(char* key)
{
	int index = hashedValue(key);
	if(array[index].flag == 0)
	{
		return NULL;
	}
	else
	{
		return array[index].value;
	}
}

