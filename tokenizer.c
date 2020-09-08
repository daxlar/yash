#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "redirection.h"
#include "pipe.h"

#define DEBUG_TOKENIZER 0

char** tokenArray;

void processCommand(char* commandString){

    int numSpaces = 0;
    for(int i = 0; commandString[i] != '\0'; i++){
        if(commandString[i] == ' '){
            numSpaces++;
        }
    }
    if(DEBUG_TOKENIZER == 1){
        printf("number of spaces: %d \n", numSpaces);
    }

    tokenArray = (char**)malloc((numSpaces + 1) * sizeof(char*));
    const char* delimiter = " ";
    char* tokenPtr = strtok(commandString, delimiter);
    int tokenCount = 0;
    while(tokenPtr != NULL){
        int tokenLength = strlen(tokenPtr);
        tokenArray[tokenCount] = (char*)malloc(tokenLength * sizeof(char));
        strcpy(tokenArray[tokenCount], tokenPtr);
        tokenPtr = strtok(NULL, delimiter);
        tokenCount++;
    }
    
    if(DEBUG_TOKENIZER == 1){
        for(int i = 0; i < tokenCount; i++){
            int stringLength = strlen(tokenArray[i]);
            printf("%s, string length: %d \n", tokenArray[i], stringLength);
        }
    }

    if(strcmp(tokenArray[1], ">") == 0){
        redirectOutputToFile(tokenArray[0], tokenArray[2]);
    }

    if(strcmp(tokenArray[1], "<") == 0){
        redirectFileToOutput(tokenArray[0], tokenArray[2]);
    }

    if(strcmp(tokenArray[1], "|") == 0){
        pipeCommands(tokenArray[0], tokenArray[2]);
    }

    for(int i = 0; i < tokenCount; i++){
        free(tokenArray[i]);
    }
    free(tokenArray);
}