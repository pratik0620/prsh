#ifndef HISTORY_H
#define HISTORY

#include<iostream>
#include<vector>
#include<fstream>

std::string getHistoryPath();
void loadHistory();
const std::vector<std::string>& getHistory();
void appendHistory(const std::string &);
std::string expandHistory(const std::string &);
void clearHistory();
void trimHistory();

#endif