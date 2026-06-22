#include<iostream>
#include <string>
#include <vector>
#include <regex>

#include "parser.h"

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

std::vector<std::string> parseRedirect(const std::string &input, CommandType type) {
    std::string command, file_name;
    size_t pos;
    int offset = 0;
    switch (type) {
        case CommandType::OUTPUT_REDIRECT:
            pos = input.find(">");
            offset = 1;
            break;

        case CommandType::APPEND_REDIRECT:
            pos = input.find(">>");
            offset = 2;
            break;

        case CommandType::INPUT_REDIRECT:
            pos = input.find("<");
            offset = 1;
            break;
        
        case CommandType::HERE_DOC:
            pos = input.find("<<");
            offset = 2;
            break;
        
        default:
            std::cerr << "Invalid redirect type\n";
            return {};
    }

    if (pos == std::string::npos) {
        std::cerr << "Operator not found in input\n";
        return {};
    }

    command = input.substr(0, pos);
    file_name = input.substr(pos + offset);

    trim(command);
    trim(file_name);

    return {command, file_name};
}

std::vector<std::string> splitPipeCommand(const std::string &input) {
    std::string str;
    std::vector<std::string> commands;
    for(char ch : input) {
        if(ch == '|') {
            trim(str);
            commands.push_back(str);
            str = "";
        } else {
            str += ch;
        }   
    }
    trim(str);
    commands.push_back(str);
    return commands;
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

bool parseBackgroundProcess(std::string &command) {
    trim(command);

    if(!command.empty() && command.back() == '&') {
        command.pop_back();
        trim(command);
        return true;
    }

    return false;
}

void expandVariables(std::string& input) {
    static const std::regex re(R"(\$([A-Za-z_][A-Za-z0-9_]*))");
    std::smatch match;

    while (std::regex_search(input, match, re)) {
        std::string var = match[1].str();

        const char* value = getenv(var.c_str());

        input.replace(
            match.position(0),
            match.length(0),
            value ? value : ""
        );
    }
}
