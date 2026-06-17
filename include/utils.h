#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

void buildArgs(const std::string[], int, char*[]);
void cleanup(char*[], int);
int buildCommandArgs(const std::string &, std::string[], char*[]);

#endif