#include<iostream>
#include<cstring>
#include<cstdlib>
#include<unistd.h>

#include "builtin.h"
#include "history.h"
#include "executor.h"

bool executeBuiltin(char* args[]) {
    if (args[0] == nullptr) {
        return true;
    }
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
        std::cout << "\033[2J\033[H" << std::flush;
        return true;
    } else if (strcmp(args[0], "history") == 0) {
        if(args[1]) {
            if(strcmp(args[1], "-c") == 0 || strcmp(args[1], "clear") == 0) {
                clearHistory();
                return true;
            }
        }

        const auto& history = getHistory();

        for(size_t i = 0; i < history.size(); i++) {
            std::cout << i + 1 << " " << history[i] << "\n";
        }

        return true;
    } else if (strcmp(args[0], "export") == 0) {
        if(args[1] == nullptr) {
            std::cerr << "env variable not specified\n";
            return true;
        }
        std::string var_name(args[1]);
        size_t delimiter_pos = var_name.find('=');
        if(delimiter_pos == std::string::npos) {
            std::cerr << "'=' is not specified\n";
            return true;
        }
        std::string env_var_name = var_name.substr(0,delimiter_pos);
        std::string env_var_value = var_name.substr(delimiter_pos + 1);

        if (setenv(env_var_name.c_str(), env_var_value.c_str(), 1) != 0) {
            perror("setenv");
        }
        return true;
    } else if (strcmp(args[0], "unset") == 0) {
        if(args[1] == nullptr) {
            std::cerr << "env variable not specified\n";
            return true;
        }

        if (unsetenv(args[1]) != 0) {
            perror("unsetenv");
        }
        return true;
    }
    
    return false;
}