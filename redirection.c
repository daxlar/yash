#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void processRedirection(char** inputTokenArray, int inputTokenArrayLength){
    
    char* command = inputTokenArray[0];
    int startOfRedirection = inputTokenArrayLength;
    int wstatus;
    for(int i = 0; i < inputTokenArrayLength; i++){
        if((strcmp(inputTokenArray[i], "<") == 0) || 
           (strcmp(inputTokenArray[i], ">") == 0) ||
           (strcmp(inputTokenArray[i],"2>") == 0)){
            startOfRedirection = i;
            break;
        }
    }

    char* stdOutFile = NULL;
    char* stdInFile = NULL;
    char* stdErrFile = NULL;

    // needs extra one space for NULL terminating argument
    char** commandArgs = (char**)malloc((startOfRedirection+1) * sizeof(char*));
    for(int i = 0; i < startOfRedirection; i++){
        commandArgs[i] = strdup(inputTokenArray[i]);
    }
    commandArgs[startOfRedirection] = NULL;

    /*
    printf("startOfRedirection: %d \n", startOfRedirection);
    for(int i = 0; i < startOfRedirection; i++){
        printf("%s \n", commandArgs[i]);
    }
    */
    
    // if there are more strings in commandArray, can't be pipe(for this function), or commands
    // must be a subset of redirections
    // move by two ( redirection , file, redirection, file, etc)
    for(int i = startOfRedirection; i < inputTokenArrayLength; i += 2){
        if(strcmp(inputTokenArray[i], ">") == 0){
            stdOutFile = inputTokenArray[i+1];
        }else if(strcmp(inputTokenArray[i], "<") == 0){
            stdInFile = inputTokenArray[i+1];
        }else if(strcmp(inputTokenArray[i], "2>") == 0){
            stdErrFile = inputTokenArray[i+1];
        }
    }

    int cpid = fork();
    if(cpid == 0){
        if(stdOutFile){
            int fd = open(stdOutFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            dup2(fd, STDOUT_FILENO);
        }
        if(stdInFile){
            int fd = open(stdInFile, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(errno == ENOENT){
                exit(0);
            }else{
                dup2(fd, STDIN_FILENO);
            }
        }
        if(stdErrFile){
            int fd = open(stdErrFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            dup2(fd, STDERR_FILENO);
        }
        execvp(command, commandArgs);
    }

    waitpid(-1, &wstatus, 0);
    for(int i = 0; i < startOfRedirection+1; i++){
        free(commandArgs[i]);
    }
    free(commandArgs);
    
}