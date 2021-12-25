#ifndef HASHMAP1_H
#define HASHMAP1_H

struct opdata
{
	char* opcode;
	int length;
};

void init_map();

void insert(char*, struct opdata*);

struct opdata* get(char*);

#endif