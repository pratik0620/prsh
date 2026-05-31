#include<iostream>
#include<string>
#include<cstring>
#include<unistd.h>
#include<sys/wait.h>

void buildArgs(std::string tokens[], int token_count, char* args[]) {
    for(size_t i=0; i<token_count; ++i) {
        args[i] = new char[tokens[i].length() + 1];
        std::strcpy(args[i], tokens[i].c_str());
    }
    args[token_count] = nullptr;
}

int tokenize(std::string &user_input, std::string tokens[]) {
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
        if(args[1] == nullptr) {
            return true;
        } else {
            if(chdir(args[1]) != 0) {
                std::cout << "Failed to change directory" << std::endl;
            }
        }
        return true; 
    }
    return false;
}

int main() {
    while(true) {
        char buffer[1024];
        getcwd(buffer, sizeof(buffer));

        std::cout << "prsh " << buffer << ">" << std::flush;
        std::string user_input;
        std::getline(std::cin, user_input);
        if(user_input.empty()) continue;

        std::string tokens[100];
        
        int token_count = tokenize(user_input, tokens);
        if(token_count == 0) continue;
        
        char *args[100];
        buildArgs(tokens, token_count, args);

        if(!executeBuiltin(args)) executeExternal(args);
        cleanup(args, token_count);
    }

    return 0;
}