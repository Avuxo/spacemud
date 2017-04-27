/*
  Client for server testing, does not have a TUI
                                                */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "test_client.h"
#include "manual.h"
#include "string_manipulation.h"

#define VERSION 1

int main(){
    int fd; /*request pipe*/
    int ret; /*response pipe*/
    int response,request; /*personal fifos to communicate with server*/

    printf("~=Space Mud=~\n");
    printf("Version: %d\n", VERSION);
    printf("Connecting...\n");
    
    char *name = requestName();
    char *res = malloc(sizeof(char) * 64); /*response*/
    /*pipe setup*/
    char *requestPipe = malloc(sizeof(char) * 16);
    char *responsePipe = malloc(sizeof(char) * 16);
    sprintf(requestPipe, "/tmp/player%sReq", name);
    sprintf(responsePipe, "/tmp/player%sRes", name);
    /*---
      Main REPL
            ---*/
    for(;;){
        char *str = prompt();
        printf("%s\n", str);
        /*check for local commands before sending to server*/
        if(strcmp(str, "help") == 0) help();
        if(strcmp(str, "quit") == 0) break;
        if(strcmp(slice(str, sizeof(str), 0, 3), "man") == 0){
            /*assigned to variable so the allocated memory can be freed*/
            char *manualEntry = man(slice(str, sizeof(str), 4, sizeof(str)));
            printf("man: %s\n", manualEntry);
            if(manualEntry) free(manualEntry);
            continue;
        };
        if(strcmp(slice(str, sizeof(str), 0, 5), "search") == 0){
            printf("...\n");
            char *res = search(slice(str, sizeof(str), 6, sizeof(str)));
            printf("Search:\n%s\n", res);
            if(res) free(res);
            continue;
        };
        
        if(strlen(str) > 8) continue;

        fd = open(requestPipe, O_WRONLY);
        write(fd, str, sizeof(str));
        
        ret = open(responsePipe, O_RDONLY);
        read(ret, res, sizeof(res)); /*read the return from the server*/
        processResponse(res);
        
        res = realloc(res, sizeof(str) * 64);
    }
    printf("Exiting...\n");
    free(requestPipe);
    free(responsePipe);
    free(res);
    free(name);
    close(fd);
    close(ret);
    return 0;
}

/*prompt user for input, sanitize and return it*/
char *prompt(){
    printf(">> ");
    char *rawInput = malloc(sizeof(char) * 128);
    scanf("%s", rawInput);
    /*sanitization of input (in theory)*/
    char *input = malloc(sizeof(char) * 64);
    strncat(input, rawInput, 63);
    return input;
}

/*request name from server*/
char *requestName(){
    char *name = malloc(sizeof(char) * 16);
    int requestFifo, resFifo;
    
    requestFifo = open("/tmp/request", O_WRONLY);
    resFifo = open("/tmp/nameRes", O_RDONLY); /*fifo for name response*/
    write(requestFifo, "JO", sizeof("JO")); /*JO is the arbitrary code for "join"*/
    
    usleep(300000); /*wait for a response*/
    read(resFifo, name, 16);
    printf("Connected with name: %s\n", name);
    return name;
}

/*process the response from server*/
void processResponse(char *input){
    printf("Response: %s\n", input);
}

/*Help menu (called by typing help)*/
void help(void){
    printf("~=Help=~\n");
    printf("Enter 'quit' to exit the game\n");
    printf("Enter 'man,[Command Sequence]' to view the manual page for a given sequence.\n");
    printf("Enter 'search,[Given search keyword]' To find the specific sequence you want.\n");
    
}
