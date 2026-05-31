#include<iostream>
#include<string>
#include<cstring>
#include<unistd.h>
#include<sys/wait.h>

int main() {
    while(true) {
        char buffer[1024];
        getcwd(buffer, sizeof(buffer));

        std::cout << "prsh " << buffer << ">" << std::flush;
        std::string user_input;
        std::getline(std::cin, user_input);
        if(user_input.empty()) continue;

        int i=0, j=0;
        std::string tokens[100];
        std::string current_token;
        while(i < user_input.size()) {
            if(user_input[i] != ' ') {
                current_token += user_input[i];
            } else {
                if(!current_token.empty()) {
                    tokens[j] = current_token;
                    current_token = "";
                    j++;
                }
            }
            i++;
        }

        if(!current_token.empty()) {
            tokens[j] = current_token;
            j++;
            current_token = "";
        }

        char *args[100];

        if(tokens[0] == "exit") break;
        else if (tokens[0] == "cd") {
            for(size_t k=0; k<j; ++k) {
                args[k] = new char[tokens[k].length() + 1];
                std::strcpy(args[k], tokens[k].c_str());
            }
            args[j] = nullptr;

            if(j < 2) {
                std::cout << "Usage: cd <directory>\n";
            } else {
                if(chdir(args[1]) == 0) {
                } else {
                    std::cout << "Failed to change directory" << std::endl;
                }
            }    
        } else {
            for(size_t k=0; k<j; ++k) {
                args[k] = new char[tokens[k].length() + 1];
                std::strcpy(args[k], tokens[k].c_str());
            }
            args[j] = nullptr;

            pid_t pid = fork();

            if(pid == 0) {
                execvp(args[0], args);
                perror("execvp failed");
                exit(1);
            } else if (pid > 0) {
                wait(NULL);
            } else {
                perror("Fork Failed");
            }
        }

        for(size_t k=0; k<j; ++k) {
            delete[] args[k];
        }
    }

    return 0;
}