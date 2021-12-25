#include <stdio.h>
#ifndef HASHMAP1_H
#define HASHMAP1_H

struct opdata
{
	char* opcode;
	int length;
};

void init_optab();

void insert_in_optab(char*, struct opdata*);
void get_optable(FILE*);
struct opdata* get(char*);

#endif