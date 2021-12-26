#include "../inc/pass1.h"
#include "../inc/pass2.h"
#include <stdio.h>
#include <string.h>

int main()
{	//file is stored in data directory
	char fn[50];
	char input_file[] = "../data/";
	int start_addr;
	printf("Enter name of input file: ");
	//input filename in data dir is fn
	scanf("%s", fn);
	strcat(input_file, fn);
	//call pass1() in pass1.c 
	pass1(input_file);
	printf("Pass 1 completed\n");
	//call pass2() in pass2.c
	pass2(input_file);
	printf("Pass 2 completed\n");
	return 0;
}
