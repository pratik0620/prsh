#ifndef HISTORY_H
#define HISTORY

#include<iostream>
#include<vector>
#include<fstream>

std::string getHistoryPath();
std::vector<std::string> readHistory();
void appendHistory(const std::string &);
std::string expandHistory(std::string &);
void clearHistory();

#endif