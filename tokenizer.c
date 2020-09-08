#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "redirection.h"
#include "pipe.h"

#define DEBUG_TOKENIZER 0

char** tokenArray;



void free2DArray(char** multiDimmensionalArray, int length){
    for(int i = 0; i < length; i++){
        free(multiDimmensionalArray[i]);
    }
    free(multiDimmensionalArray);
}


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

    /*
    if(strcmp(tokenArray[1], ">") == 0){
        redirectOutputToFile(tokenArray[0], tokenArray[2]);
    }

    if(strcmp(tokenArray[1], "<") == 0){
        redirectFileToOutput(tokenArray[0], tokenArray[2]);
    }

    if(strcmp(tokenArray[1], "|") == 0){
        pipeCommands(tokenArray[0], tokenArray[2]);
    }
    */

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
            // first position is going to be the command
            commandOne = tokenArray[0];
            // account for the beginning command
            int commandOneArgsLength = pipeTokenPosition-1;
            commandOneArgs = (char**)malloc(commandOneArgsLength * sizeof(char*));
            for(int i = 1; i < pipeTokenPosition; i++){
                strcpy(commandOneArgs[i-1], tokenArray[i]);
            }

            commandTwo = tokenArray[pipeTokenPosition+1];
            // tokenCount is 1 greater than last index of tokenArray 
            // and account for command right after pipe
            int commandTwoArgsLength = tokenCount-pipeTokenPosition-2;
            commandTwoArgs = (char**)malloc(commandTwoArgsLength * sizeof(char*));
            int commandTwoArgsCounter = 0;
            for(int i = pipeTokenPosition+2; i < tokenCount; i++){
                strcpy(commandTwoArgs[commandTwoArgsCounter], tokenArray[i]);
            }

            // pipe manipulation here


            // free memory here
            free2DArray(commandOneArgs, commandOneArgsLength);
            free2DArray(commandTwoArgs, commandTwoArgsLength);
        }else{
            processRedirectionNoPipe(tokenArray, tokenCount);
        }
    }


    for(int i = 0; i < tokenCount; i++){
        free(tokenArray[i]);
    }
    free(tokenArray);
}