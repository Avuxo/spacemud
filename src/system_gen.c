/*---
  Code for System and galaxy generation.
                                     ---*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "server.h"
#include "system_gen.h"
#include "string_manipulation.h"

/*generate a system struct*/
System generateSystem(int ID){
    System ret;
    ret.ID = ID;
    ret.name = nameGenerator();
    return ret;
}


/*NAME GENERATION*/
/*random words in English, Russian and giberish*/
char *words[] = {"Abolish", "Carrat", "Adventure", "Xray", "Hallucinate", "Keen", "Pixie", "Zeal", "Zeitgiest", "Realistic", "Quantity", "Quantum", "Galaxy", "yazika", "opredollom", "kaktisya", "Pygorax", "Temper", "Voytek", "Zashtar", "Alkarana", "Ablaka", "yabloki", "Oodachi", "Character", "Chance", "izvinitye", "Karashonta", "Katyusha"};

const char *vowels[] = {"a", "e", "i", "o", "u"};

/*temporary name generator*/
char *nameGenerator(void){

    /*split the first word in half*/
    int first = rand() % 29;
    char *firstHalf = slice(words[first], sizeof(words[first]),
                            0, (strlen(words[first])) >> 1);
    
    /*split the second word in half*/
    int second;
    if(first > 10)
        second = first - 5;
    else
        second = first + 5;          
    char *secondHalf = slice(words[second], sizeof(words[second]),
                             ((strlen(words[second])) >> 1) + 1, strlen(words[second]));

    char *name = malloc(sizeof(char) * 16);
    
    /*stick a vowel inbetween the two word for added readability*/
    int vowel = rand() % 5;
    name = strcat(firstHalf, vowels[vowel]);
    
    /*combine the two words to get the name.*/
    name = strcat(name, secondHalf);
    return name;
}

