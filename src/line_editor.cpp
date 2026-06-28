#include<iostream>
#include<string>
#include<unistd.h>
#include<thread>
#include<chrono>

#include "terminal.h"
#include "line_editor.h"
#include "constants.h"

std::string readLine() {
    enableRawMode();

    std::string buffer;

    while(true) {
        char ch;
        ssize_t n = read(STDIN_FILENO, &ch, 1);

        if(n<=0) {
            disableRawMode();
            return "";
        }

        if(ch == '\n' || ch == '\r') {
            std::cout << "\n";
            std::cout << buffer << std::endl;
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
                        std::cout << "Arrow UP Key detected\r\n";
                        break;
                        case 'B': 
                        std::cout << "Arrow DOWN Key detected\r\n"; 
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