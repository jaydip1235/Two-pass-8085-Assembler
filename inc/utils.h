#ifndef UTILS_H
#define UTILS_H

long long int string_to_int(char*);
char* to_hex(int);
void get_optable(FILE*);
void get_symtable(FILE*);
char* get_mnemonic(char *line);
char* read_line(FILE*);
char* check_label(char*);
int insert_label(char*, int, FILE*);
int has_label(char*, FILE*);
char* get_operand(char* , char*);
int is_valid_operand(char*, int);
char **get_operands(char*, int);

#endif
