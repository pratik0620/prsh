#include<iostream>
#include<cstring>
#include<unistd.h>
#include<sys/wait.h>

#include "executor.h"

void executeCommand(char* args[]) {
    execvp(args[0], args);
    perror("execvp failed");
    exit(1);
}
