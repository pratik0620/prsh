#include<iostream>
#include<vector>
#include<array>
#include<sys/wait.h>

#include "pipe.h"
#include "utils.h"
#include "constants.h"

void executePipe(const std::vector<std::string> &commands) {
    if(commands.empty()) return;
    int N = commands.size();

    for(const auto &command : commands) {
        if(command.empty()) {
            std::cerr << "syntax error near unexpected token '|'\n";
            return;
        }
    }

    std::vector<std::array<int, 2>> fd(N-1);

    for(int i=0; i<N-1; i++) {
        if(pipe(fd[i].data()) == -1) {
            perror("pipe");
            return ;
        }
    }

    std::vector<pid_t> pid(N);

    for(int i=0; i<N; i++) {
        pid[i] = fork();

        if(pid[i] < 0) {
            perror("Fork Failed");
            for(int i=0; i<N-1; i++) {
                close(fd[i][0]);
                close(fd[i][1]);
            }
            for(int k=0; k<i; k++) {
                waitpid(pid[k], NULL, 0);
            }
            return;
        } else if (pid[i] == 0) {
            if(i != 0) {
                if (dup2(fd[i-1][0], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(1);
                }
            }

            if(i != N-1) {
                if (dup2(fd[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(1);
                }
            }

            for(int j=0; j<N-1; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
            }

            std::string tokens[MAX_ARGS];            
            char *args[MAX_ARGS];
            int token_count = buildCommandArgs(commands[i], tokens, args);

            execvp(args[0], args);
            cleanup(args, token_count);
            perror("execvp failed");
            exit(1);
        }
    }

    for(int i=0; i<N-1; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    for(int i=0; i<N; i++) {
        waitpid(pid[i], NULL, 0);
    }
}