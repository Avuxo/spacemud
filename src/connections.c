#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "server.h"
#include "connections.h"

/*initialze a connection struct*/
io_handler *initConnections(int ID){
    io_handler *handler = malloc(sizeof(handler));
    handler->inName = malloc(sizeof(char) * 20);
    handler->outName = malloc(sizeof(char) * 20);
    handler->set = 1;
    sprintf(handler->outName, "/tmp/player%dRes", ID);
    sprintf(handler->inName, "/tmp/player%dReq", ID);

    return handler;
}

/*free the connection struct*/
void freeConnection(io_handler *handler){
    handler->set = 0;
    free(handler->inName);
    free(handler->outName);
}

