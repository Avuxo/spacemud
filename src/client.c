#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <ncurses.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#include "client.h"

/*MACROS*/
#define MAX_X 64
#define MAX_Y 25

int stars[8] = {3,8,10,13,2,9,5,7};

int main(int argc, char **argv){

    pthread_t windowThread;
    pthread_t promptThread;
    
    initscr();
    pthread_create(&promptThread, NULL, prompt, "...");

    pthread_join(promptThread, NULL);
    getch();
    endwin();

    pthread_cancel(promptThread);
    return 0;
}


/*
Draw the window
 */
void drawWindow(){
    clear();
    
    /*Draws window frame*/
    for(int i=1; i<MAX_X; i++){
        mvprintw(1,i, "-");
    }
    for(int i=2; i<MAX_Y; i++){
        mvprintw(i,1,  "|");
        mvprintw(i,(MAX_X-1), "|");
    }
    for(int i=1; i<MAX_X; i++){
        mvprintw((MAX_Y-1), i, "-");
    }
    for(int i=1; i<MAX_X; i++){
        mvprintw((MAX_Y-3), i, "-");
    }
    /*Draw the stars*/
    for(int i=3; i<(MAX_Y-4); i++){
        mvprintw(i, stars[i-4], "*");
    }
        
    /*Draw the info from the server*/
    /*getFromServer() or something which then prints the info it returns*/
    
    usleep(50000); /*make thread sleep so it doesnt fucking die*/
    
}

void *prompt(){
    for(;;){
        clear();
        drawWindow();
        mvprintw((MAX_Y-2),2, "> ");
        char input[32];
        getnstr(input, 32);
        usleep(5000);
    }
    
    /*All commands executed are to be in the following format:
     > three letter order (mov for move to etc) first_arg(2 characters) 
                                                second_arg(2 characters, 00for nothing)
     Example:
     > mov $1
    */
}
