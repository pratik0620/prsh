#ifndef REDIRECT_H
#define REDIRECT_H

#include <string>
#include <vector>

#include "types.h"

void executeRedirect(const std::string &, RedirectType, int, int);
void executeHereDoc(const std::string &, CommandType);

#endif