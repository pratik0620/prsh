#include<iostream>

#include<string>
#include<cstring>

#include<unistd.h>
#include<sys/wait.h>

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

int main() {
    while(true) {
        char buffer[1024];
        getcwd(buffer, sizeof(buffer));

        std::cout << "prsh " << buffer << ">" << std::flush;
        std::string user_input;
        std::getline(std::cin, user_input);
        if(user_input.empty()) continue;

        if(isPipe(user_input)) {
            std::string left = "", right = "";
            splitCommand(user_input, left, right);

            std::string leftTokens[100], rightTokens[100];
            int leftTokenCount = tokenize(left, leftTokens);
            int rightTokenCount = tokenize(right, rightTokens);

            char *left_args[100], *right_args[100];

            buildArgs(leftTokens, leftTokenCount, left_args);
            buildArgs(rightTokens, rightTokenCount, right_args);

            cleanup(left_args, leftTokenCount);
            cleanup(right_args, rightTokenCount);
        } else {
            std::string tokens[100];

            int token_count = tokenize(user_input, tokens);
            if(token_count == 0) continue;
            
            char *args[100];
            buildArgs(tokens, token_count, args);

            if(!executeBuiltin(args)) executeExternal(args);
            cleanup(args, token_count);
        }
    }

    return 0;
}