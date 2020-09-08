#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



void pipeCommands(char* shellCommand1, char* shellCommand2){

    //fork twice to create two child processes

    int pipefd[2];
    int wstatus;

    pipe(pipefd);

    int cpid = fork();
    if(cpid == 0){
        close(pipefd[0]);
        dup2(pipefd[1],STDOUT_FILENO);
        execlp(shellCommand1, shellCommand1, (char *) NULL);
    }

    cpid = fork();
    if(cpid == 0){
        close(pipefd[1]);
        dup2(pipefd[0],STDIN_FILENO);
        execlp(shellCommand2, shellCommand2, (char *) NULL);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(-1, &wstatus, 0);
    waitpid(-1, &wstatus, 0);
}