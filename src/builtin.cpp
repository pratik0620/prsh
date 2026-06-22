#include<iostream>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<sys/wait.h>

#include "builtin.h"
#include "history.h"
#include "executor.h"

bool executeBuiltin(char* args[]) {
    if(strcmp(args[0], "exit") == 0) {
        std::cout << "\n";
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
    } else if (strcmp(args[0], "history") == 0) {
        if(args[1]) {
            if(strcmp(args[1], "-c") == 0 || strcmp(args[1], "clear") == 0) {
                clearHistory();
                return true;
            }
        }

        std::vector<std::string> history = readHistory();

        for(size_t i = 0; i < history.size(); i++) {
            std::cout << i + 1 << " " << history[i] << "\n";
        }

        return true;
    }
    
    return false;
}