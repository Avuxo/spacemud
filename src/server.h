#define MAX_PLAYERS 64 /*Max possible connected players - I really don't recommend going beyond this number due to the fact that this is written using a thread for each player (foolishly)'*/
#define SYSTEMS_PER_GALAXY 10
#define INVENTORY_SIZE 16
#include "item.h"

typedef struct {
    char *system; /*the current system*/
    int systemId; /*ID of current system*/
    int x; /*local x coordinate*/
    int y; /*local y coordinate*/
    int fuel; /*the players fuel*/
    int ID; /*shared ID with client*/
    Item items[INVENTORY_SIZE];
} Player;

/*return type of the getLocation function*/
typedef struct {
    int x;
    int y;
} Point;

typedef struct{
    Point shops[5]; /*shops in system*/
    Point players[MAX_PLAYERS]; /*players in system*/
    int ID;
    char *name;
} System;

typedef struct{
    System systems[SYSTEMS_PER_GALAXY];
    int ID;
    char *name;
} Galaxy;

/*function definitions*/
//void playerConnect(char *name, int *numConnected);
void playerDisconnect(int ID);
char *processInput(char *input, int playerID);
int movePlayer(int newX, int newY, int playerID);
int jumpSystem(int newSysID, int playerID); /*jumps a player to a diff system*/
void *waitForPlayers();
void *waitForInput(void *ID);
Point getLocation(int playerID);
Player initPlayer(int ID);
void addPlayer(Player player, int playerID);
void setConnections();
int *playersInSystem(int systemID);
void sendToSystem(char *msg, int systemID);
