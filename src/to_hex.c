#include <stdio.h>
#include <stdlib.h>
#include "../inc/to_hex.h"
// Function to convert decimal to hexadecimal 
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
        //printf("temp %d\n", temp);
        if(temp < 10) 
        { 
            hexa[i] = temp + 48; 
        } 
        else
        { 
            hexa[i] = temp + 55; 
        } 
         
        i--;  
        n = n / 16; 
    } 
      
    // printing hexadecimal number array in reverse order 
    for(int j = i; j >= 0; j--) 
    {
        hexa[j] = '0';
    } 
    hexa[4] = '\0';
  
    return hexa;
} 
