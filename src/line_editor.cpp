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
    size_t cursor_pos = 0;

    const auto& history = getHistory();
    int historyIndex = static_cast<int>(history.size());

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
            if (!buffer.empty() && cursor_pos > 0) {
                buffer.erase(cursor_pos - 1, 1);
                cursor_pos--;
                refreshLine(prompt, buffer, cursor_pos);
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
                            cursor_pos = buffer.size();
                            refreshLine(prompt, buffer, cursor_pos);
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
                            cursor_pos = buffer.size();
                            refreshLine(prompt, buffer, cursor_pos);
                        }
                        break;
                        
                        case 'C':
                        if (cursor_pos < buffer.size()) {
                            cursor_pos++;
                            std::cout << "\033[C" << std::flush;
                        }
                        break;

                        case 'D': 
                        if(cursor_pos > 0) {
                            cursor_pos--;
                            std::cout << "\033[D" << std::flush;
                        }
                        break;
                    }
                }
            }
            continue;
        }

        buffer.insert(cursor_pos, 1, ch);
        cursor_pos++;

        refreshLine(prompt, buffer, cursor_pos);
    }
}

void refreshLine(const std::string& prompt, const std::string& buffer, size_t cursor_pos) {
    write(STDOUT_FILENO, "\r\033[2K", 5);

    std::cout << prompt << buffer << std::flush;

    size_t move_left = buffer.size() - cursor_pos;

    while(move_left--) {
        std::cout << "\033[D";
    }

    std::cout << std::flush;
}