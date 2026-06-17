#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

#include "types.h"

CommandType getCommandType(const std::string &);
void trim(std::string &);
void splitCommand(const std::string &, std::string &, std::string &, CommandType);
std::vector<std::string> splitPipeCommand(const std::string &);

int tokenize(const std::string &, std::string[]);

#endif