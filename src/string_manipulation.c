#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "string_manipulation.h"

/*take a given string and return a substring given a start and end point*/
char *slice(char *input, size_t size, int start, int end){
    char *dest = malloc(size);
    memmove(dest, input+start, end);
    return dest;
}

/*convert given string to uppercase*/
char *toUpperCase(char *input){
    char *retString = malloc(sizeof(char) * strlen(input));
    for(int i=0; i<strlen(input); i++){
        retString[i] = (char)toupper(input[i]);
    }
    return retString;
}
