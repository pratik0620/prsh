#include<iostream>

#include "history.h"
#include "constants.h"

std::string getHistoryPath() {
    const char* home = getenv("HOME");

    if(home == nullptr) return ".prsh_history";

    return std::string(home) + "/.prsh_history";
}

std::vector<std::string> readHistory() {
    std::vector<std::string> history;
    std::string history_path = getHistoryPath();

    std::ifstream file(history_path);

    if(!file) {
        return history;
    }

    std::string line;
    while(std::getline(file, line)){
        history.push_back(line);
    }

    return history;
}

void appendHistory(const std::string & command) {
    std::string history_path = getHistoryPath();

    std::ofstream file(history_path, std::ios::app);

    if(!file) {
        std::cerr << "Failed to open history file\n";
        return;
    }

    file << command << '\n';
}

std::string expandHistory(std::string &input) {
    std::vector<std::string> history = readHistory();

    if(history.empty()) {
        return "";
    }

    if(input == "!!") {
        return history.back();
    }

    if(input.rfind("!-", 0) == 0) {
        int n = std::stoi(input.substr(2));

        if(n < 1 || n > history.size()) {
            return "";
        }

        return history[history.size() - n];
    }

    if(input.size() > 1 && input[0] == '!') {
        int index = std::stoi(input.substr(1));

        if(index < 1 || index > history.size()) {
            return "";
        }

        return history[index - 1];
    }

    return input;
}

void clearHistory() {
    std::string history_path = getHistoryPath();

    std::ofstream file(history_path, std::ios::trunc);

    if(!file) {
        std::cerr << "Failed to clear history\n";
    }
}

void trimHistory() {
    std::vector<std::string> history = readHistory();

    if(history.size() <= MAX_HISTORY_SIZE) {
        return;
    }

    std::string history_path = getHistoryPath();
    std::ofstream file(history_path, std::ios::trunc);

    size_t start = history.size() - MAX_HISTORY_SIZE;

    for(size_t i = start; i<history.size(); i++) {
        file << history[i] << "\n";
    }
}