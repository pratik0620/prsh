#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

#include "types.h"

CommandType getCommandType(const std::string &);
void trim(std::string &);
std::vector<std::string> parseRedirect(const std::string &, CommandType);
std::vector<std::string> splitPipeCommand(const std::string &);
bool parseBackgroundProcess(std::string &);
void expandVariables(std::string &);

int tokenize(const std::string &, std::string[]);

#endif