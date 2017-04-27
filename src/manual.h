#include <stdarg.h> /*using this is dirty : /*/

/*experimental way of handling commands*/
typedef struct {
    char *code; /*the Code value of the command*/
    char *command; /*what the command actually is*/
    void (*method)(void *args, ...); /*the method that the code calls*/
} Command;


char *man(char *code); /* manual for given code*/
char *search(char *name); /*search for a code given a word*/
int isCommand(char *cmd);
char *execCommand(char *cmd, char *arg); /*execute commands*/
