#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>

#include "redirect.h"
#include "parser.h"
#include "utils.h"
#include "constants.h"

void executeRedirect(const std::string &user_input, RedirectType type, int open_flag, int target_fd) {
    std::string left, right;
    switch (type) {
        case RedirectType::OUTPUT:
            splitCommand(user_input, left, right, CommandType::OUTPUT_REDIRECT);
            break;

        case RedirectType::APPEND:
            splitCommand(user_input, left, right, CommandType::APPEND_REDIRECT);
            break;

        case RedirectType::INPUT:
            splitCommand(user_input, left, right, CommandType::INPUT_REDIRECT);
            break;
        
        default:
            std::cerr << "Invalid redirect type\n";
            return;
    }

    std::string leftTokens[MAX_ARGS];
    char *left_args[MAX_ARGS];
    int leftTokenCount = buildCommandArgs(left, leftTokens, left_args);

    int fd = open(right.c_str(), open_flag, 0644);
    if(fd == -1) {
        perror("open");
        cleanup(left_args, leftTokenCount);
        return;
    }

    pid_t pid = fork();
            
    if(pid < 0) {
        perror("Fork Failed");
    } else if(pid == 0) {
        if (dup2(fd, target_fd) == -1) {
            perror("dup2");
            exit(1);
        }
        close(fd);

        execvp(left_args[0], left_args);
        perror("execvp failed");
        cleanup(left_args, leftTokenCount);
        exit(1);
    } 
    close(fd);
    cleanup(left_args, leftTokenCount);
    waitpid(pid, NULL, 0);
}

void executeHereDoc(const std::string &user_input, CommandType type) {
    int fd[2];

    if(pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    std::string left, right;
    splitCommand(user_input, left, right, type);

    std::string leftTokens[MAX_ARGS];
    char *left_args[MAX_ARGS];
    int leftTokenCount = buildCommandArgs(left, leftTokens, left_args);

    std::string input ;
    while(true) {
        std::getline(std::cin, input);
        if(input == right) break;
        input += '\n';
        if (write(fd[1], input.c_str(), input.size()) == -1) {
            perror("write");
            exit(1);    
        };
    }

    close(fd[1]);

    pid_t pid = fork();

    if(pid < 0) {
        perror("Fork Failed");
        close(fd[0]);
        exit(1);
    } else if (pid == 0) {
        if(dup2(fd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(fd[0]);

        execvp(left_args[0], left_args);
        perror("execvp failed");
        cleanup(left_args, leftTokenCount);
        exit(1);
    }
    close(fd[0]);
    cleanup(left_args, leftTokenCount);
    waitpid(pid, NULL, 0);
}
