#include<iostream>
#include<string>
#include<unistd.h>

#include "terminal.h"
#include "line_editor.h"
#include "constants.h"
#include "history.h"

std::string readLine(const std::string& prompt) {
    enableRawMode();

    std::string buffer;

    std::vector<std::string> history = readHistory();
    int historyIndex = history.size();

    while(true) {
        char ch;
        ssize_t n = read(STDIN_FILENO, &ch, 1);

        if(n<=0) {
            disableRawMode();
            return "";
        }

        if(ch == '\n' || ch == '\r') {
            std::cout << "\n";
            disableRawMode();
            return buffer;
        }

        if (ch == BACKSPACE) {
            if (!buffer.empty()) {
                buffer.pop_back();
                std::cout << "\b \b" << std::flush;
            }
            continue;
        }

        if (ch == ESC) {
            char seq[2];
            if(read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1) {
                if (seq[0] == '[') {
                    switch(seq[1]) {
                        case 'A': 
                        if(!history.empty() && historyIndex > 0) {
                            historyIndex--;
                            buffer = history[historyIndex];
                            refreshLine(prompt, buffer);
                        }
                        break;

                        case 'B': 
                        if(historyIndex < history.size()) {
                            historyIndex++;
                            if (historyIndex == history.size()) {
                                buffer.clear();
                            }
                            else {
                                buffer = history[historyIndex];
                            }
                            refreshLine(prompt, buffer);
                        }
                        break;
                        
                        case 'C': 
                        std::cout << "Arrow RIGHT Key detected\r\n"; 
                        break;
                        case 'D': 
                        std::cout << "Arrow LEFT Key detected\r\n"; 
                        break;
                    }
                }
            }
            continue;
        }

        std::cout << ch << std::flush;
        buffer += ch;
    }
}

void refreshLine(const std::string& prompt, const std::string& buffer) {
    write(STDOUT_FILENO, "\r\033[2K", 5);

    std::cout << prompt << buffer << std::flush;
}