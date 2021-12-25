#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../inc/utils.h"
#include "../inc/hash_map_1.h"
#include "../inc/hash_map_2.h"

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
char* to_hex(int n) 
{   

    if(n > 65535)
    {
        perror("Memory error"); 
        return NULL;
    }

    // Char array to store hexadecimal number 
    char* hexa = (char*)malloc(sizeof(char) * 5); 

    int i = 3; 
    while(n != 0) 
    {     
        int temp  = n % 16; 
        if(temp < 10) 
        { 
            hexa[i] = temp + '0'; 
        } 
        else
        { 
            hexa[i] = temp-10 +'A'; 
        } 
         
        i--;  
        n /= 16; 
    } 
      
    // add preceeding zeros
    for(int j = i; j >= 0; j--) 
    {
        hexa[j] = '0';
    } 
    hexa[4] = '\0';
    return hexa;
} 
char* remove_char(char *line,char c)
{
	// removes c from line
	char *line2;
	int i=0,j=0,len;
	len=strlen(line);
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
	//like mov a,b
	if(get(line2) != NULL) {return line2;}
	//get first token/word
	char delim[]=" ";
	line3=strtok(line5,delim);
	if(get(line3) != NULL) {return line3;}
	//for immediate addressing
	char delim2[]=",";
	line4=strtok(line2,delim2);
	if(get(line4) != NULL) {return line4;}
	else
	{
		return NULL;
	}
}

char* read_line(FILE* fp)
{	//returns a line from the input file
	int pos = 0;
	char* buff = (char*)malloc(sizeof(char) * 200);
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
	//returns the label if the line starts with a label else NULL
	int flag = 0; // Flag indicating whether label exists
	int pos;
	// mark flag true if : is found and update pos as the index of the ':'
	for(int i = 0; line[i] != '\0'; i++)
	{
		if(line[i] == ':')
		{
			pos = i;
			flag = 1;
			break; 
		}
	}

	if(flag)
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
		insert_in_symtab(label, loc_hex);
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