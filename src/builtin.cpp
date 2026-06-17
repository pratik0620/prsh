#include<iostream>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<sys/wait.h>

#include "builtin.h"

bool executeBuiltin(char* args[]) {
    if(strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        if(args[1] == nullptr || strcmp(args[1], "~") == 0) {
            chdir(getenv("HOME"));
        } else {
            if(chdir(args[1]) != 0) {
                perror("cd");
            }
        }
        return true; 
    } else if (strcmp(args[0], "clear") == 0) {
        std::cout << "\033[2J\033[H";
        return true;
    }
    return false;
}