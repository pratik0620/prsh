#ifndef REDIRECT_H
#define REDIRECT_H

#include <string>
#include <vector>

#include "types.h"

std::vector<std::string> executeRedirect(const std::string &, RedirectType, int, int);
std::vector<std::string> executeHereDoc(const std::string &, CommandType);

#endif