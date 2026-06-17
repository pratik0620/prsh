#include<iostream>
#include<cstring>

#include "utils.h"
#include "parser.h"

void buildArgs(const std::string tokens[], int token_count, char* args[]) {
    for(size_t i=0; i<token_count; ++i) {
        args[i] = new char[tokens[i].length() + 1];
        std::strcpy(args[i], tokens[i].c_str());
    }
    args[token_count] = nullptr;
}

void cleanup(char* args[], int count){
    for(size_t i=0; i<count; ++i) delete[] args[i];
}

int buildCommandArgs(const std::string &command, std::string tokens[], char* args[]) {
    int token_count = tokenize(command, tokens);
    buildArgs(tokens, token_count, args);
    return token_count;
}