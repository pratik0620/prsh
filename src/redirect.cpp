#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>

#include "redirect.h"
#include "parser.h"
#include "utils.h"
#include "constants.h"
#include "executor.h"

std::vector<std::string> executeRedirect(const std::string &user_input, RedirectType type, int open_flag, int target_fd) {
    std::vector<std::string> result;
    switch (type) {
        case RedirectType::OUTPUT:
            result = parseRedirect(user_input, CommandType::OUTPUT_REDIRECT);
            break;

        case RedirectType::APPEND:
            result = parseRedirect(user_input, CommandType::APPEND_REDIRECT);
            break;

        case RedirectType::INPUT:
            result = parseRedirect(user_input, CommandType::INPUT_REDIRECT);
            break;
        
        default:
            std::cerr << "Invalid redirect type\n";
            return {};
    }

    int fd = open(result[1].c_str(), open_flag, 0644);
    if(fd == -1) {
        perror("open");
        return {};
    }

    if (dup2(fd, target_fd) == -1) {
        perror("dup2");
        exit(1);
    }
    close(fd);

    return result;
}

std::vector<std::string> executeHereDoc(const std::string &user_input, CommandType type) {
    int fd[2];

    if(pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    std::vector<std::string> result = parseRedirect(user_input, type);

    std::string input ;
    while(true) {
        std::getline(std::cin, input);
        if(input == result[1]) break;
        input += '\n';
        if (write(fd[1], input.c_str(), input.size()) == -1) {
            perror("write");
            exit(1);
        };
    }
    close(fd[1]);

    if(dup2(fd[0], STDIN_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }
    close(fd[0]);

    return result;
}
