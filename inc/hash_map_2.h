#ifndef HASHMAP2_H
#define HASHMAP2_H

struct data{
	char* key;
	char* value;
	int flag ;
};

void initialize_map();

void insert_in_map(char*, char*);

char* get_value(char*);

#endif