#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

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
        int fd = open(fileName, O_WRONLY | O_APPEND | O_CREAT, 0644);
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
        int fd = open(fileName, O_RDONLY | O_APPEND | O_CREAT, 0644);
        int newFd = dup2(fd, STDIN_FILENO);
        int errorValue = execlp(shellCommand, shellCommand, (char*)NULL);
    }else{
        waitpid(-1, &wstatus, 0);
    }
}