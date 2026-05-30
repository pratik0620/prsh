#include<iostream>
#include<string>
#include<cstring>

int main() {
    while(true) {
        std::cout << "prsh>" << std::flush;
        std::string user_input;
        std::getline(std::cin, user_input);
        if(user_input.empty()) continue;

        char *args[100];

        int i=0, j=0;
        std::string tokens[100];
        std::string current_token;
        while(user_input[i] != '\0') {
            if(user_input[i] != ' ') {
                current_token += user_input[i];
            } else {
                if(current_token.empty()) {
                    i++;
                    continue;
                }
                tokens[j] = current_token;
                current_token = "";
                j++;
            }
            i++;
        }

        tokens[j] = current_token;
        current_token = "";
        
        for(int k = 0; k <= j; k++) {
            std::cout << "Token " << k << ": " << tokens[k] << '\n';
        }
    }
}