typedef struct{
    int in;        /*fifo for requests*/
    int out;       /*fifo for responses*/
    int set;       /*1 for exists, 2 for nonexistant*/
    int ID;        /*ID of user*/
    char *inName;  /*name of request fifo*/
    char *outName; /*name of response fifo*/
} io_handler;

io_handler *initConnections(int ID);
void freeConnections(io_handler *handler);
