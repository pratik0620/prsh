#include<iostream>
#include <fstream>
#include <string>
#include <vector>

#include "history.h"
#include "constants.h"

static std::vector<std::string> history;

std::string getHistoryPath() {
    const char* home = getenv("HOME");

    if(home == nullptr) return ".prsh_history";

    return std::string(home) + "/.prsh_history";
}

void loadHistory() {
    history.clear();

    std::string history_path = getHistoryPath();
    std::ifstream file(history_path);

    if(!file) {
        return;
    }

    std::string line;
    while(std::getline(file, line)){
        history.push_back(line);
    }

    trimHistory();
}

const std::vector<std::string>& getHistory() {
    return history;
}

void appendHistory(const std::string& command) {
    history.push_back(command);
    if (history.size() > MAX_HISTORY_SIZE) {
        trimHistory();
        return;
    }

    std::string history_path = getHistoryPath();

    std::ofstream file(history_path, std::ios::app);

    if(!file) {
        std::cerr << "Failed to open history file\n";
        return;
    }

    file << command << '\n';
}

std::string expandHistory(const std::string &input) {
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
    history.clear();
}

void trimHistory() {
    if(history.size() <= MAX_HISTORY_SIZE) {
        return;
    }

    std::string history_path = getHistoryPath();
    std::ofstream file(history_path, std::ios::trunc);

    if (!file) {
        std::cerr << "Failed to rewrite history file\n";
        return;
    }

    for (const auto& command : history) {
        file << command << '\n';
    }

    history.erase(history.begin(), history.begin() + (history.size() - MAX_HISTORY_SIZE));
}