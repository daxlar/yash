#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tokenizer.h"

int main(int argc, char** argv){

    const char* prompt = "# ";
    char* result;
    
    while(result = readline(prompt)){
        
        processCommand(result);
        free(result);
    }

    return 0;
}