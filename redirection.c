#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


/**
 * fork and create child
 * redirect in fdt of child
 * execute shellCommand in terms of child
 **/


/**
 *  shellCommand > fileName
 **/
void redirectOutputToFile(char* shellCommand, char* fileName){

    int cpid;
    int wstatus;

    cpid = fork();
    if(cpid == 0){
        int fd = open(fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        int newFd = dup2(fd, STDOUT_FILENO);
        int errorValue = execlp(shellCommand, shellCommand, (char*)NULL);
    }else{
        waitpid(-1, &wstatus, 0);
    }
}

/**
 *  shellCommand < fileName
 **/
void redirectFileToOutput(char* shellCommand, char* fileName){
    
    int cpid;
    int wstatus;

    cpid = fork();
    if(cpid == 0){
        int fd = open(fileName, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(fd < 0){
            if(errno == ENOENT){
                char* errorMessage = "bash: fakefile.txt: No such file or directory\n";
                int errMessageLength = strlen(errorMessage);
                write(STDERR_FILENO, errorMessage, errMessageLength);
            }
        }else{
            int newFd = dup2(fd, STDIN_FILENO);
            int errorValue = execlp(shellCommand, shellCommand, (char*)NULL);
        }
    }else{
        waitpid(-1, &wstatus, 0);
    }
}


void processRedirectionNoPipe(char** commandArray, int arrayLength){
    char* command = commandArray[0];
    int startOfRedirection = arrayLength;
    int wstatus;
    for(int i = 0; i < arrayLength; i++){
        if((strcmp(commandArray[i], "<") == 0) || 
           (strcmp(commandArray[i], ">") == 0) ||
           (strcmp(commandArray[i],"2>") == 0)){
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
        commandArgs[i] = strdup(commandArray[i]);
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
    for(int i = startOfRedirection; i < arrayLength; i += 2){
        if(strcmp(commandArray[i], ">") == 0){
            stdOutFile = commandArray[i+1];
        }else if(strcmp(commandArray[i], "<") == 0){
            stdInFile = commandArray[i+1];
        }else if(strcmp(commandArray[i], "2>") == 0){
            stdErrFile = commandArray[i+1];
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