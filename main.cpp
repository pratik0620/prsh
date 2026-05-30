#include<iostream>
#include<string>
#include<cstring>

int main() {
    while(true) {
        std::cout << "prsh>" << std::flush;
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

        for(size_t k=0; k<j; ++k) {
            args[k] = new char[tokens[k].length() + 1];
            std::strcpy(args[k], tokens[k].c_str());
        }
        args[j] = nullptr;

        for(size_t k=0; k<j; ++k) {
            delete[] args[k];
        }
    }

    return 0;
}