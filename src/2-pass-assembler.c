#include "../inc/pass1.h"
#include "../inc/pass2.h"
#include <stdio.h>
#include <string.h>

int main()
{
	char ip[50];
	char input_file[] = "../data/";
	printf("Enter name of input file: ");
	scanf("%s", ip);
	strcat(input_file, ip);
	pass1(input_file);
	printf("Pass 1 completed\n");
	pass2(input_file);
	printf("Pass 2 completed\n");
	return 0;
}
