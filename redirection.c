#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


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
        int fd = open(fileName, O_WRONLY | O_CREAT, 0644);
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
        int newFd = dup2(fd, STDIN_FILENO);
        int errorValue = execlp(shellCommand, shellCommand, (char*)NULL);
    }else{
        waitpid(-1, &wstatus, 0);
    }
}