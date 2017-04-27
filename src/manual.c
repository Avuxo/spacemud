#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "manual.h"
#include "string_manipulation.h"

#define NUM_OF_CODES 11
#define STR_LENGTH 128

/*LUT for the codes*/
static char codes[NUM_OF_CODES][3][STR_LENGTH] ={
    /*ship codes*/
    {"M04", "Get coordinates of closest object"},
    {"M0C", "Search current system for objects"},
    {"M01", " M01xy - Move to position (x,y) - where x & y are hexidecimal digits"},
    {"M02", "What is my current location"},
    
    /*communication codes*/
    {"0045", "I come in peace."},
    {"1132", "Repeat your last."},

    /*return codes*/
    {"R000", "Unknown command"},
    {"R001", "Successfully moved"},
    {"R002", "Unable to move"},
    {"RFFF", "Unknown error."},
    {"R1", "Location of request is (x,y) - R1xy"}
};

static Command commands[NUM_OF_CODES];

/*the man command - manual*/
char *man(char *code){
    char *ret = malloc(sizeof(char) * 128);
    for(int i=0; i<NUM_OF_CODES; i++){
        if(strcmp(code, codes[i][0]) == 0){
            sprintf(ret, "%s - %s", codes[i][0], codes[i][1]);
            return ret;
        }
    }
    /*prevents a segfault upon free()ing*/
    sprintf(ret, "ERR (invalid command)");
    return ret;
}

/*the search command - search for a given code with a keyword*/
char *search(char *name){
    char *search = malloc(sizeof(char) * 2048);
    char *buf_tmp = malloc(sizeof(char) * 128); /*temp buffer for loading each command*/
    for(int i=0; i<NUM_OF_CODES; i++){
        if(!codes[i][1][0]) break;
        char *check = strstr(codes[i][1], name); /*check if substring exists*/
        if(check){
            sprintf(buf_tmp, "%s - %s", codes[i][0], codes[i][1]); /*string of possible cmds*/
            sprintf(search + strlen(search), "%s", buf_tmp);
        }
    }
    free(buf_tmp);

    return search;
}

/*verify that a command indeed exists*/
int isCommand(char *cmd){
    if(strcmp(slice(cmd, sizeof(cmd), 0, 3), "M01") == 0) return 1;
    
    for(int i=0; i<NUM_OF_CODES; i++){
        if(strcmp(cmd, codes[i][0]) == 0){
            return 1;
        }
    }
    return 0;
}

/*TODO: implement this function after commands are switched to struct format*/
char *execCommand(char *cmd, char *arg){
    for(int i=0; i<NUM_OF_CODES; i++){
        if(strcmp(cmd, commands[i].code) == 0){
            void *t;
            commands[i].method("...");
        }
    }
    return "R000";
}
