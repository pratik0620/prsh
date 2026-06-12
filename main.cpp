#include<iostream>

#include<string>
#include<cstring>

#include<cstdlib>

#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>

constexpr int MAX_ARGS = 100;
constexpr int MAX_PATH_LENGTH = 1024;

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

bool isPipe(const std::string &user_input) {
    int i=0;
    while(i < user_input.size()) {
        if(user_input[i] == '|')
            return true;
        i++;
    }
    return false;
}

void splitCommand(const std::string &user_input, std::string &left, std::string &right) {
    int i=0;
    while(user_input[i] != '|') {
        left += user_input[i++];
    }
    i++;
    while(i < user_input.size()) {
        right += user_input[i++];
    }
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

bool isRedirection(const std::string &user_input) {
    int i=0;
    while(i < user_input.size()) {
        if(user_input[i] == '>')
            return true;
        i++;
    }
    return false;
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

        if(isPipe(user_input)) {
            std::string left = "", right = "";
            splitCommand(user_input, left, right);

            std::string leftTokens[MAX_ARGS], rightTokens[MAX_ARGS];
            int leftTokenCount = tokenize(left, leftTokens);
            int rightTokenCount = tokenize(right, rightTokens);

            char *left_args[MAX_ARGS], *right_args[MAX_ARGS];
            
            buildArgs(leftTokens, leftTokenCount, left_args);
            buildArgs(rightTokens, rightTokenCount, right_args);

            executePipe(left_args, right_args);

            cleanup(left_args, leftTokenCount);
            cleanup(right_args, rightTokenCount);
        } else if (isRedirection(user_input)) {
            pid_t pid = fork();
            
            if(pid < 0) {
                perror("Fork Failed");
            } else if(pid == 0) {
                std::string left = "", right = "";
                int i=0;
                while(user_input[i] != '>') {
                    left += user_input[i++];
                }
                i++;
                while(i < user_input.size()) {
                    right += user_input[i++];
                }

                std::string leftTokens[MAX_ARGS], rightTokens[MAX_ARGS];
                int leftTokenCount = tokenize(left, leftTokens);
                int rightTokenCount = tokenize(right, rightTokens);

                char *left_args[MAX_ARGS], *right_args[MAX_ARGS];
                
                buildArgs(leftTokens, leftTokenCount, left_args);
                buildArgs(rightTokens, rightTokenCount, right_args);

                int fd = open(right_args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if(fd == -1) {
                    perror("open");
                    exit(1);
                }

                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(1);
                }
                close(fd);

                execvp(left_args[0], left_args);
                perror("execvp failed");
                cleanup(left_args, leftTokenCount);
                cleanup(right_args, rightTokenCount);
                exit(1);
            }
            waitpid(pid, NULL, 0);
        } else {
            std::string tokens[MAX_ARGS];

            int token_count = tokenize(user_input, tokens);
            if(token_count == 0) continue;
            
            char *args[MAX_ARGS];
            buildArgs(tokens, token_count, args);

            if(!executeBuiltin(args)) executeExternal(args);
            cleanup(args, token_count);
        }
    }

    return 0;
}