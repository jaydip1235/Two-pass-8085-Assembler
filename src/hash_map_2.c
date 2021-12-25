#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../inc/hash_map_2.h"

unsigned long long int string_to_int_convert(char* input)
{
	int i =0;
	unsigned long long int result = 0;
	int r = 0;
	
	while(input[i]!='\0')
	{
		int temp = input[i];
		result += temp * pow(100,r);
		r++;
		i++;
	}
	return result;
}

static struct data *array;
static int capacity = 19997;

int hashedValue(char* key)
{
	unsigned long long int result = string_to_int_convert(key);
	return (result % capacity);
}

void initialize_map()
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

void insert_in_map(char* key, char* value)
{
	int index = hashedValue(key);
	array[index].key = (char*)malloc(30 * sizeof(char));
	array[index].value = (char*)malloc(5 * sizeof(char));
	strcpy(array[index].key, key);
	strcpy(array[index].value,value);
	array[index].flag = 1;
}

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

