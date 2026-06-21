#include<iostream>
#include<cstring>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>

#include "executor.h"

void executeCommand(char* args[]) {
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    execvp(args[0], args);
    perror("execvp failed");
    exit(1);
}
