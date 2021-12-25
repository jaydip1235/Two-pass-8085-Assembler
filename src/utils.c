#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../inc/utils.h"
#include "../inc/to_hex.h"
#include "../inc/hash_map_1.h"
#include "../inc/hash_map_2.h"

void get_optable(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	int igr;
	char mnemonic[10];
	char opcode[3];
	int length;

	init_map();
	/* Reading line from file until EOF */
	while(fscanf(fp,"%d. %s %s %d",&igr,mnemonic,opcode,&length)!=EOF)
	{
		/* Inserting opcodes into hash table */
		struct opdata opd;
		opd.length = length;
		opd.opcode = (char*)malloc(sizeof(char) * 3);
		strcpy(opd.opcode, opcode);
		insert(mnemonic, &opd);

	}
}

void get_symtable(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);

	initialize_map();
	char label[30];
	char address[5];
	/* Reading line from file until EOF */
	while(fscanf(fp,"%s %s",label, address)!=EOF)
	{
		/* Inserting symbols into hash table */
		insert_in_map(label, address);
	}
}

char* remove_char(char *line,char c)
{
	char *line2;
	int i=0,j=0,len;
	len=strlen(line);
	//printf("%d\n",len);
	line2=(char*)malloc(len*sizeof(char));
	for(i=0;i<len;i++)
	{
		if(line[i]!=c) {line2[j]=line[i];j++;}
	}
	line2[j]='\0';
	return line2;
}

char* get_mnemonic(char *line)
{
	
 	char *line2,*line3,*line4, *line5;
 	line5 = (char*)malloc(10*sizeof(char));
 	strcpy(line5, line);
	line2=remove_char(line,' ');

	if(get(line2) != NULL) {return line2;}
	char delim[]=" ";
	line3=strtok(line5,delim);
	if(get(line3) != NULL) {return line3;}
	char delim2[]=",";
	line4=strtok(line2,delim2);
	if(get(line4) != NULL) {return line4;}
	else
	{
		return NULL;
	}
}

char* read_line(FILE* fp)
{
	int bufsize = 1024;
	int pos = 0;
	char* buff = (char*)malloc(sizeof(char) * bufsize);
	char ch = getc(fp);
	while(ch != '\n' && ch != EOF)
	{
		buff[pos] = ch;
		ch = getc(fp);
		pos++;
	}
	buff[pos] = '\0';
	return buff;
}

char* check_label(char* line)
{
	int f = 0; // Flag indicating whether label exists
	int pos;
	/*  Checking for label */
	for(int i = 0; line[i] != '\0'; i++)
	{
		if(line[i] == ':')
		{
			pos = i;
			f = 1;
			break; 
		}
	}

	if(f == 1)
	{
		int i;
		/* Storing the label */
		char* label = (char*)malloc(sizeof(char) * 30);
		for(i = 0; i < pos; i++)
		{
			label[i] = line[i];
		}
		label[pos] = '\0';

		/* Storing the instruction */
		for(i = pos + 1; line[i] != '\0'; i++)
		{
			line[i - pos - 1] = line[i];
		}

		line[i - pos - 1] = '\0';

		return label;
	}
	else
	{
		return NULL;
	}
}

int insert_label(char* label, int locctr, FILE* fp)
{
	//cout << "Label: " << label << endl;
	if(get_value(label) != NULL)
	{
		return 0;
	}

	else
	{
		char* loc_hex = to_hex(locctr);
		insert_in_map(label, loc_hex);
		fprintf(fp, "%s %s\n", label, loc_hex);
		return 1;
	}
}

int has_label(char* mnemonic, FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	char m[10];
	while(fscanf(fp, "%s\n", m) != EOF)
	{	
		if(strcmp(m, mnemonic) == 0)
		{
			return 1;
		}
	}

	return 0;
}

char* get_operand(char* line, char* mnemonic)
{
	char* line2 = remove_char(line, ' ');
	int i, pos = strlen(mnemonic);

	char *operand = (char*)malloc(sizeof(char) * 10);
	int j = 0;
	for(i = pos; line2[i] != '\0'; i++)
	{
		if(i == pos && line2[i] == ',')
			continue;

		operand[j] = line2[i];
		j++;
	}
	operand[j] = '\0';

	return operand;
}

int is_hexa_and_remove_h(char *op)
{
	if(strlen(op) != 0)
	{
		int i,length=strlen(op);
		for(i=0;i<length-1;i++)
		{
			if(!((op[i]>='0' && op[i]<='9') || (op[i]>='a' && op[i]<='f') || (op[i]>='A' && op[i]<='F'))) return 0; 
		}
		if((op[i]>='0' && op[i]<='9') || (op[i]>='a' && op[i]<='f') || (op[i]>='A' && op[i]<='F') ||op[i]=='h' || op[i]=='H')
		{
			if(op[i]=='h' || op[i]=='H') op[i]='\0';
			return 1;
		}
		return 0;
	}
	return 1;
}

int is_valid_operand(char *operand,int length)
{
	int h=is_hexa_and_remove_h(operand);
	if(!h) return 0;
	return (2*length==strlen(operand));
}

char **get_operands(char *operand,int length)
{
	char **opr;
	opr=(char**)(malloc(2*sizeof(char*)));
	opr[0]=(char*)(malloc(3*sizeof(char)));
	opr[1]=(char*)(malloc(3*sizeof(char)));

	if(length==0) return NULL;
	if(length==1) strcpy(opr[0], operand);
	else 
	{
		int i,j;
		for(i=0;i<=1;i++)
		{
			for(j=0;j<=1;j++)
			{
				opr[i][j]=operand[2*(1-i)+j];
			}
		}
		opr[0][2]='\0';
		opr[1][2]='\0';
	}
	return opr;
}