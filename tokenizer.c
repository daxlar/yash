#include <stdio.h>
#include <string.h>
#include <stdlib.h>



char** tokenArray;


void processCommand(char* commandString){

    int numSpaces = 0;
    for(int i = 0; commandString[i] != '\0'; i++){
        if(commandString[i] == ' '){
            numSpaces++;
        }
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

    for(int i = 0; i < tokenCount; i++){
        printf("%s\n", tokenArray[i]);
    }

    for(int i = 0; i < tokenCount; i++){
        free(tokenArray[i]);
    }
    free(tokenArray);
}