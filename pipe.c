#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PIPE_DEBUGGING 0

void processPipe(char** inputTokenArray, int inputTokenArrayLength){

    //process the inputTokenArray into commands and command args
    char* commandOne;
    char* commandTwo;
    char** commandOneArgs;
    char** commandTwoArgs;

    int pipeTokenIndex = 0;
    for(int i = 0; i < inputTokenArrayLength; i++){
        if(strcmp(inputTokenArray[i], "|") == 0){
            pipeTokenIndex = i;
            break;
        }
    }
    if(PIPE_DEBUGGING == 1){
        printf("pipeTokenIndex: %d\n", pipeTokenIndex);
    }

    char* commandOneStdInFile = NULL;
    char* commandOneStdOutFile = NULL;
    char* commandOneStdErrFile = NULL;
    char* commandTwoStdInFile = NULL;
    char* commandTwoStdOutFile = NULL;
    char* commandTwoStdErrFile = NULL;

    int firstRedirectionIndex = pipeTokenIndex;
    for(int i = 0; i < pipeTokenIndex; i++){
        if((strcmp(inputTokenArray[i], "<") == 0) || 
           (strcmp(inputTokenArray[i], ">") == 0) ||
           (strcmp(inputTokenArray[i], "2>") == 0)){
               firstRedirectionIndex = i;
               break;
        }
    }
    commandOne = inputTokenArray[0];
    if(PIPE_DEBUGGING == 1){
        printf("firstRedirectionIndex: %d\n", firstRedirectionIndex);
    }
    int commandOneArgsLength = firstRedirectionIndex + 1;
    if(PIPE_DEBUGGING == 1){
        printf("commandOneArgsLength: %d\n", commandOneArgsLength);
    }
    commandOneArgs = (char**)malloc(commandOneArgsLength * sizeof(char*));
    for(int i = 0; i < firstRedirectionIndex; i++){
        commandOneArgs[i] = strdup(inputTokenArray[i]);
    }
    commandOneArgs[firstRedirectionIndex] = NULL;
    if(PIPE_DEBUGGING == 1){
        for(int i = 0; i < commandOneArgsLength; i++){
            printf("commandOneArgs: %s\n", commandOneArgs[i]);
        }
    }
    for(int i = firstRedirectionIndex; i < pipeTokenIndex; i += 2){
        if(strcmp(inputTokenArray[i], ">") == 0){
            commandOneStdOutFile = inputTokenArray[i+1];
        }else if(strcmp(inputTokenArray[i], "<") == 0){
            commandOneStdInFile = inputTokenArray[i+1];
        }else if(strcmp(inputTokenArray[i], "2>") == 0){
            commandOneStdErrFile = inputTokenArray[i+1];
        }
    }

    int secondRedirectionIndex = inputTokenArrayLength;
    for(int i = pipeTokenIndex + 1; i < inputTokenArrayLength; i++){
        if((strcmp(inputTokenArray[i], "<") == 0) || 
           (strcmp(inputTokenArray[i], ">") == 0) ||
           (strcmp(inputTokenArray[i], "2>") == 0)){
               secondRedirectionIndex = i;
               break;
        }
    }
    commandTwo = inputTokenArray[pipeTokenIndex+1];
    if(PIPE_DEBUGGING == 1){
        printf("secondRedirectionIndex: %d\n", secondRedirectionIndex);
    }
    int commandTwoArgsLength = secondRedirectionIndex - pipeTokenIndex;
    if(PIPE_DEBUGGING == 1){
        printf("commandTwoArgsLength: %d\n", commandTwoArgsLength);
    }
    commandTwoArgs = (char**)malloc(commandTwoArgsLength * sizeof(char*));
    for(int i = 0, j = pipeTokenIndex + 1; j < secondRedirectionIndex; i++, j++){
        commandTwoArgs[i] = strdup(inputTokenArray[j]);
    }
    commandTwoArgs[commandTwoArgsLength-1] = NULL;
    if(PIPE_DEBUGGING == 1){
        for(int i = 0; i < commandTwoArgsLength; i++){
            printf("commandTwoArgs: %s\n", commandTwoArgs[i]);
        }
    }
    for(int i = secondRedirectionIndex; i < inputTokenArrayLength; i += 2){
        if(strcmp(inputTokenArray[i], ">") == 0){
            commandTwoStdOutFile = inputTokenArray[i+1];
        }else if(strcmp(inputTokenArray[i], "<") == 0){
            commandTwoStdInFile = inputTokenArray[i+1];
        }else if(strcmp(inputTokenArray[i], "2>") == 0){
            commandTwoStdErrFile = inputTokenArray[i+1];
        }
    }
    if(PIPE_DEBUGGING == 1){
        printf("commandOneStdInFile: %s\n", commandOneStdInFile);
        printf("commandOneStdOutFile: %s\n", commandOneStdOutFile);
        printf("commandOneStdErrFile: %s\n", commandOneStdErrFile);
        printf("commandTwoStdInFile: %s\n", commandTwoStdInFile);
        printf("commandTwoStdOutFile: %s\n", commandTwoStdOutFile);
        printf("commandTwoStdErrFile: %s\n", commandTwoStdErrFile);
    }

    //fork twice to create two child processes
    int pipefd[2];
    int wstatus;

    pipe(pipefd);

    int cpid = fork();
    if(cpid == 0){
        close(pipefd[0]);
        dup2(pipefd[1],STDOUT_FILENO);
        if(commandOneStdOutFile){
            int fd = open(commandOneStdOutFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            dup2(fd, STDOUT_FILENO);
        }
        if(commandOneStdInFile){
            int fd = open(commandOneStdInFile, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(errno == ENOENT){
                exit(0);
            }else{
                dup2(fd, STDIN_FILENO);
            }
        }
        if(commandOneStdErrFile){
            int fd = open(commandOneStdErrFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            dup2(fd, STDERR_FILENO);
        }
        execvp(commandOne, commandOneArgs);
    }

    cpid = fork();
    if(cpid == 0){
        close(pipefd[1]);
        dup2(pipefd[0],STDIN_FILENO);
        if(commandTwoStdOutFile){
            int fd = open(commandTwoStdOutFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            dup2(fd, STDOUT_FILENO);
        }
        if(commandTwoStdInFile){
            int fd = open(commandTwoStdInFile, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(errno == ENOENT){
                exit(0);
            }else{
                dup2(fd, STDIN_FILENO);
            }
        }
        if(commandTwoStdErrFile){
            int fd = open(commandTwoStdErrFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            dup2(fd, STDERR_FILENO);
        }
        execvp(commandTwo, commandTwoArgs);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(-1, &wstatus, 0);
    waitpid(-1, &wstatus, 0);

    for(int i = 0; i < commandOneArgsLength; i++){
        free(commandOneArgs[i]);
    }
    free(commandOneArgs);

    for(int i = 0; i < commandTwoArgsLength; i++){
        free(commandTwoArgs[i]);
    }
    free(commandTwoArgs);

}