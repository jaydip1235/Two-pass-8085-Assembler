#include <stdio.h>
#ifndef HASHMAP2_H
#define HASHMAP2_H

struct data{
	char* key;
	char* value;
	int flag ;
};

void init_symtab();

void insert_in_symtab(char*, char*);
void get_symtable(FILE*);
char* get_value(char*);

#endif