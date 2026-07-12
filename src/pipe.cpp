#include<iostream>
#include<vector>
#include<array>
#include<sys/wait.h>
#include<fcntl.h>

#include "pipe.h"
#include "utils.h"
#include "constants.h"
#include "executor.h"
#include "parser.h"
#include "redirect.h"
#include "types.h"
#include "builtin.h"
#include "jobs.h"

void executePipe(const std::vector<std::string> &commands, bool background) {
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
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            
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

            std::string command = commands[i];

            CommandType command_type = getCommandType(commands[i]);

            std::vector<std::string> result;
            switch (command_type) {
                case CommandType::OUTPUT_REDIRECT:
                    result = executeRedirect(commands[i], RedirectType::OUTPUT, O_WRONLY | O_CREAT | O_TRUNC, STDOUT_FILENO);
                    if(result.empty()) {
                        exit(1);
                    }
                    command = result[0];
                    break;

                case CommandType::APPEND_REDIRECT:
                    result = executeRedirect(commands[i], RedirectType::APPEND, O_WRONLY | O_CREAT | O_APPEND, STDOUT_FILENO);
                    if(result.empty()) {
                        exit(1);
                    }
                    command = result[0];
                    break;

                case CommandType::INPUT_REDIRECT:
                    result = executeRedirect(commands[i], RedirectType::INPUT, O_RDONLY, STDIN_FILENO);
                    if(result.empty()) {
                        exit(1);
                    }
                    command = result[0];
                    break;

                case CommandType::HERE_DOC:
                    result = executeHereDoc(commands[i], CommandType::HERE_DOC);
                    if(result.empty()) {
                        exit(1);
                    }
                    command = result[0];
                    break;

                default:
                    break;
            }

            int token_count = buildCommandArgs(command, tokens, args);
            if(!executeBuiltin(args)) executeCommand(args);
            cleanup(args, token_count);
            exit(0);
        }
    }

    if(background) {
        addJob(pid[N-1], commands.back(), JobStatus::RUNNING);
        std::cout << "[PID] " << pid[N-1] << std::endl;
    }

    for(int i=0; i<N-1; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    for(int i=0; i<N; i++) {
        if(!background) {
            int status;
            if (waitpid(pid[i], &status, WUNTRACED) == -1) {
                perror("waitpid");
                continue;
            }

            if(WIFSTOPPED(status)) {
                addJob(pid[i], commands[i], JobStatus::STOPPED);
                std::cout << "\n[PID " << pid[i] << "] stopped\n";
            }
        }
    }
}