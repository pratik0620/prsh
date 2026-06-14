#include<iostream>

#include<string>
#include<cstring>

#include<cstdlib>

#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>

constexpr int MAX_ARGS = 100;
constexpr int MAX_PATH_LENGTH = 1024;

enum class CommandType {
    NORMAL,
    PIPE, 
    OUTPUT_REDIRECT, 
    APPEND_REDIRECT, 
    INPUT_REDIRECT, 
    HERE_DOC
};

enum class RedirectType {
    OUTPUT,
    APPEND,
    INPUT
};

CommandType getCommandType(const std::string &input) {
    if(input.find("|") != std::string::npos) return CommandType::PIPE;
    if(input.find(">>") != std::string::npos) return CommandType::APPEND_REDIRECT;
    if(input.find("<<") != std::string::npos) return CommandType::HERE_DOC;
    if(input.find(">") != std::string::npos) return CommandType::OUTPUT_REDIRECT;
    if(input.find("<") != std::string::npos) return CommandType::INPUT_REDIRECT;
    return CommandType::NORMAL;
};

void trim(std::string &str) {
    const std::string whitespace = " \t\n\r\f\v";
    
    size_t end = str.find_last_not_of(whitespace);
    if (end != std::string::npos) {
        str.erase(end + 1);
    } else {
        str.clear();
        return;
    }
    
    size_t start = str.find_first_not_of(whitespace);
    if (start != std::string::npos) {
        str.erase(0, start);
    }
}

void buildArgs(const std::string tokens[], int token_count, char* args[]) {
    for(size_t i=0; i<token_count; ++i) {
        args[i] = new char[tokens[i].length() + 1];
        std::strcpy(args[i], tokens[i].c_str());
    }
    args[token_count] = nullptr;
}

int tokenize(const std::string &user_input, std::string tokens[]) {
    std::string current_token;
    int i=0, token_count=0;
    while(i < user_input.size()) {
        if(user_input[i] != ' ') {
            current_token += user_input[i];
        } else {
            if(!current_token.empty()) {
                tokens[token_count++] = current_token;
                current_token = "";
            }
        }
        i++;
    }
    if(!current_token.empty()) {
        tokens[token_count++] = current_token;
        current_token = "";
    }
    return token_count;
}

void cleanup(char* args[], int count){
    for(size_t i=0; i<count; ++i) delete[] args[i];
}

int buildCommandArgs(const std::string &command, std::string tokens[], char* args[]) {
    int token_count = tokenize(command, tokens);
    buildArgs(tokens, token_count, args);
    return token_count;
}

void executeExternal(char* args[]) {
    pid_t pid = fork();

    if(pid == 0) {
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("Fork Failed");
        return;
    }
}

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

void splitCommand(const std::string &user_input, std::string &left, std::string &right, CommandType type) {
    size_t pos;
    int offset = 0;
    switch (type) {
        case CommandType::PIPE:
            pos = user_input.find("|");
            offset = 1;
            break;

        case CommandType::OUTPUT_REDIRECT:
            pos = user_input.find(">");
            offset = 1;
            break;

        case CommandType::APPEND_REDIRECT:
            pos = user_input.find(">>");
            offset = 2;
            break;

        case CommandType::INPUT_REDIRECT:
            pos = user_input.find("<");
            offset = 1;
            break;
        
        case CommandType::HERE_DOC:
            pos = user_input.find("<<");
            offset = 2;
            break;
        
        default:
            std::cerr << "Invalid redirect type\n";
            return;
    }

    left = user_input.substr(0, pos);
    right = user_input.substr(pos + offset);

    trim(left);
    trim(right);
}

void executePipe(char* left_args[], char* right_args[]) {
    int fd[2];

    if(pipe(fd) == -1) {
        perror("pipe");
        return ;
    }

    pid_t pid1 = fork();

    if(pid1 < 0) {
        perror("Fork Failed");
        close(fd[0]);
        close(fd[1]);
        return;
    }

    if(pid1 == 0) {
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(fd[0]);
        close(fd[1]);
        execvp(left_args[0], left_args);
        perror("execvp failed");
        exit(1);
    }   

    pid_t pid2 = fork();

    if(pid2 < 0) {
        perror("Fork Failed");
        close(fd[0]);
        close(fd[1]);
        waitpid(pid1, NULL, 0);
        return;
    }

    if(pid2 == 0) {
        if (dup2(fd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(fd[1]);
        close(fd[0]);
        execvp(right_args[0], right_args);
        perror("execvp failed");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

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

int main() {
    while(true) {
        char buffer[MAX_PATH_LENGTH];
        if(getcwd(buffer, sizeof(buffer)) == nullptr) {
            perror("getcwd");
            continue;
        }

        std::cout << "prsh " << buffer << ">" << std::flush;
        std::string user_input;
        std::getline(std::cin, user_input);
        if(user_input.empty()) continue;

        CommandType command_type = getCommandType(user_input);

        switch (command_type) {
        case CommandType::PIPE: {
            std::string left = "", right = "";
            splitCommand(user_input, left, right, CommandType::PIPE);

            std::string leftTokens[MAX_ARGS], rightTokens[MAX_ARGS];
            char *left_args[MAX_ARGS], *right_args[MAX_ARGS];
            
            int leftTokenCount = buildCommandArgs(left, leftTokens, left_args);
            int rightTokenCount = buildCommandArgs(right, rightTokens, right_args);

            executePipe(left_args, right_args);

            cleanup(left_args, leftTokenCount);
            cleanup(right_args, rightTokenCount);
            break;
        }

        case CommandType::OUTPUT_REDIRECT:
            executeRedirect(user_input, RedirectType::OUTPUT, O_WRONLY | O_CREAT | O_TRUNC, STDOUT_FILENO);
            break;

        case CommandType::APPEND_REDIRECT:
            executeRedirect(user_input, RedirectType::APPEND, O_WRONLY | O_CREAT | O_APPEND, STDOUT_FILENO);
            break;

        case CommandType::INPUT_REDIRECT:
            executeRedirect(user_input, RedirectType::INPUT, O_RDONLY, STDIN_FILENO);
            break;

        case CommandType::HERE_DOC: {
            int fd[2];

            if(pipe(fd) == -1) {
                perror("pipe");
                exit(1);
            }

            std::string left, right;
            splitCommand(user_input, left, right, CommandType::HERE_DOC);

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

            break;
        }

        case CommandType::NORMAL: {
            std::string tokens[MAX_ARGS];            
            char *args[MAX_ARGS];
            int token_count = buildCommandArgs(user_input, tokens, args);

            if(!executeBuiltin(args)) executeExternal(args);
            cleanup(args, token_count);
            
            break;
        }

        default:
            break;
        }
    }

    return 0;
}