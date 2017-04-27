/*
  Server for space-mud
                      */
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#include "string_manipulation.h"
#include "server.h"
#include "system_gen.h"
#include "manual.h"
#include "connections.h"

/*GLOBALL VARIABLES*/
Galaxy galaxy; /*main galaxy*/
Player players[MAX_PLAYERS]; /*all currently connected players*/

io_handler *connections[MAX_PLAYERS]; /*all connections for players*/

void freeConnection(io_handler *handler); /*redefined to avoid compiler warning*/

int main(int argc, char **argv){
    srand(time(NULL)); /*set random seed*/

    int fdJoin, joinRes; /*test pipe*/
    int tokenRes, tokenReq;
    
    /*FIFOs for name requests (when user joins)*/
    mkfifo("/tmp/request", 0666); /*name request pipe*/
    mkfifo("/tmp/nameRes", 0666); /*name response pipe*/
    
    /*create FIFO pipes for each user*/
    mkfifo("/tmp/join", 0666); /*test pipe*/
    mkfifo("/tmp/joinRes", 0666); /*test reponse pipe*/

    setConnections();
    
    /*thread to wait for players*/
    pthread_t waitThread;
    pthread_create(&waitThread, NULL, waitForPlayers, "...");

    pthread_t inputThreads[MAX_PLAYERS];
    for(int i=0; i<MAX_PLAYERS; i++){
        int *arg = malloc(sizeof(*arg)); /*allocate argument*/
        if(arg==NULL){
            printf("Could not allocate arguments for thread creation (thread %d).\n", i);
            continue;
        }
        *arg = i; /*send playerID as argument*/
        pthread_create(&inputThreads[i], NULL, waitForInput, arg);
    }

    /*wait for threads to close before continuing*/
    for(int i=0; i<MAX_PLAYERS; i++){
        pthread_join(inputThreads[i], NULL);
    }

    close(fdJoin);
    return 0;
}

/*create in and out pipes for users*/
void setConnections(){
    for(int i=0; i<MAX_PLAYERS; i++){
        io_handler *temp = malloc(sizeof(temp));
        temp->set = 0;
        connections[i] = temp;
    }
}

/*threaded function to wait for user input*/
void *waitForInput(void *playerID){
    int ID = *((int *) playerID);
    free(playerID);

    char *buf; /*buffer for player input*/
    buf = malloc(sizeof(char) * 32);
    for(;;){
        
        /*if there is no user on this thread do nothing.*/
        if(!connections[ID]->set){
            usleep(30000);
            continue;
        }

        /*open in and out FIFOs*/
        connections[ID]->in = open(connections[ID]->inName, O_RDONLY);
        connections[ID]->out = open(connections[ID]->outName, O_WRONLY);

        /*read user input*/
        read(connections[ID]->in, buf, sizeof(buf));
        
        if(strlen(buf) > 8) continue; /*make sure the command isnt too large*/
        if(!buf[0]) continue; /*make sure the command exists*/
        buf = toUpperCase(buf); /*convert to uppercase*/
        printf("%d> %s\n", ID, buf);

        /*create and write a resposne*/
        char *res = processInput(buf, ID);
        write(connections[ID]->out, res, sizeof(res));
        
        usleep(30000); /*slow down thread*/
    }
    free(buf);
}


/*waits for players and deals with people when they join*/
void *waitForPlayers(void){
    int requestFifo, resFifo;    
    char *requestStream = malloc(sizeof(char) * 32);
    int playersConnected = 0;

    /*make sure all player fifos are intialized*/
    char *fifoName_tmp = malloc(sizeof(char) * 32); /*temporary buffer for names*/
    for(int i=0; i<MAX_PLAYERS; i++){
        sprintf(fifoName_tmp, "/tmp/player%dRes", i);
        mkfifo(fifoName_tmp, 0666);
        sprintf(fifoName_tmp, "/tmp/player%dReq", i);
        mkfifo(fifoName_tmp, 0666);
    }
    free(fifoName_tmp);
    
    printf("Waiting for players...\n");
    char *name = malloc(sizeof(char) * 8);
    for(;;){
        requestFifo = open("/tmp/request", O_RDONLY); /*name request FIFO*/
        resFifo = open("/tmp/nameRes", O_WRONLY); /*name response FIFO*/
        
        if((playersConnected+1 >= 64)){
            continue;
        }

        read(requestFifo, requestStream, sizeof(requestStream)); /*read for requests*/
        printf("Player Connected.\n");
        
        /*add player*/
        addPlayer(initPlayer(playersConnected+1), playersConnected+1);

        connections[playersConnected+1] = initConnections(playersConnected);
        playersConnected++;
        /*give new player a name*/
        sprintf(name, "%d", (playersConnected - 1));

        write(resFifo, name, sizeof(name));

        usleep(30000);
    }

    /*free player fifos*/
    for(int i=0; i<playersConnected; i++){
        freeConnection(connections[i]);
    }
    free(name);
    free(requestStream);
    close(requestFifo);
}

/*process user input*/
char *processInput(char *input, int playerID){
    /*check if the command is for ship, broadcast or else*/
    
    /*the command type*/
    char *cmd = slice(input, sizeof(input), 0, 3);
    /*first argument*/
    char *arg1 = slice(input, sizeof(input), 4, 5);
    char *response;

    if(!isCommand(input)) return "R000"; /*make sure input is a command*/
    
    /*check for commands*/

    if(strcmp(cmd, "M01") == 0){
        if(!input[3] || !input[4]){
            response = "R002";
        }
        /*convert the char to an int*/
        int newX = input[3] - '0';
        int newY = input[4] - '0';

        if(movePlayer(newX, newY, playerID)) response = "R001";
        else response = "R002";
    }else if(strcmp(cmd, "M02") == 0){
        printf("Get loc\n");
        Point location = getLocation(playerID);
        char *loc = malloc(sizeof(char) * 8);
        sprintf(loc, "R1%d%d", location.x, location.y);
        response = loc;
    }else{
        response = "RFFF";
    }
    free(arg1);

    return response;
}

/*sends a message to all users in a given system*/
void sendToSystem(char *msg, int systemID){
    int *playerIDs = playersInSystem(systemID);
    int len = sizeof(playerIDs)/sizeof(int); /*num players*/
    int res; /*response pipe*/
    
    char *buf_tmp = malloc(sizeof(char) * 32); /*temporary buffer for pipe name*/
    for(int i=0; i<len; i++){
        /*put the pipe name into the buffer*/
        sprintf(buf_tmp, "/tmp/player%dRes", playerIDs[i]);
        res = open(buf_tmp, O_WRONLY);
        write(res, msg, sizeof(msg));
        close(res);
    }
    free(buf_tmp);
    free(playerIDs);
}

/*return array of IDs of players in system*/
int *playersInSystem(int systemID){
    int *playerIDs = malloc(sizeof(int) * MAX_PLAYERS);
    for(int i=0; i<MAX_PLAYERS; i++){
        int playerCount = 0;
        if(players[i].systemId == systemID){
            playerIDs[playerCount] = players[i].ID;
            playerCount++;
        }
    }
}

/*move a player to a given location*/
int movePlayer(int newX, int newY, int playerID){
    /*avoid overflow*/
    if(playerID > MAX_PLAYERS) return 0;
    
    /*calculate distance*/
    double distance = 2; /*Temporary testing distance - will be variable later on*/

    if(players[playerID].fuel < distance) return 0;/*check fuel for travel*/
    /*make sure the move is valid*/
    if(players[playerID].x == newX && players[playerID].y == newY) return 0;

    /*move the player and subtract from their fuel*/
    players[playerID].x = newX;
    players[playerID].y = newY;
    players[playerID].fuel--;
    printf("Moved %d To (%d,%d). Player now has %d fuel\n", playerID,
           players[playerID].x, players[playerID].y, players[playerID].fuel);
    return 1;
}

/*get a given player's location*/
Point getLocation(int playerID){
    Point ret;
    ret.x = players[playerID].x;
    ret.y = players[playerID].y;
    return ret;
}

void addPlayer(Player player, int playerID){
    players[playerID] = player;
}

/*initialize a player*/
Player initPlayer(int ID){
    Player player;
    player.systemId = 0;
    player.system = "";
    player.x = 0;
    player.y = 0;
    player.fuel = 10;
    player.ID = ID;
    return player;
}

/*when a player disconnects*/
void playerDisconnect(int ID){
    freeConnection(connections[ID]);
    printf("Player %d disconnected\n", ID);
}
