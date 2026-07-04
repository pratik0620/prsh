#ifndef LINE_EDITOR_H
#define LINE_EDITOR_H

#include<string>
#include "types.h"

ReadResult readLine(const std::string&);
void refreshLine(const std::string& , const std::string&, size_t);

#endif