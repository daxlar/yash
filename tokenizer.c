#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "redirection.h"
#include "pipe.h"

#define DEBUG_TOKENIZER 0

char** tokenArray;

void processInput(char* inputString){

    int numSpaces = 0;
    for(int i = 0; inputString[i] != '\0'; i++){
        if(inputString[i] == ' '){
            numSpaces++;
        }
    }
    if(DEBUG_TOKENIZER == 1){
        printf("number of spaces: %d \n", numSpaces);
    }

    tokenArray = (char**)malloc((numSpaces + 1) * sizeof(char*));
    const char* delimiter = " ";
    char* tokenPtr = strtok(inputString, delimiter);
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

    // we know that the first string must be a valid command
    // pipes need to spawn two children
    // organize into command blocks that are sent into either end of a pipe that exists
    if(tokenCount > 0){
        char* commandOne;
        char** commandOneArgs;
        char* commandTwo;
        char** commandTwoArgs;

        // look for pipe first
        int pipeTokenPosition = -1;
        for(int i = 0; i < tokenCount; i++){
            if(strcmp(tokenArray[i], "|") == 0){
                pipeTokenPosition = i;
            }
        }

        if(pipeTokenPosition > 0){
            processPipe(tokenArray, tokenCount);
        }else{
            processRedirection(tokenArray, tokenCount);
        }
    }


    for(int i = 0; i < tokenCount; i++){
        free(tokenArray[i]);
    }
    free(tokenArray);
}