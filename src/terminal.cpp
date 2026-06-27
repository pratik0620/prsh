#include <unistd.h>
#include<termios.h>

#include "terminal.h"

struct termios original_settings;


void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &original_settings) == -1) {
        perror("tcgetattr");
        return;
    }
    
    struct termios raw = original_settings;

    raw.c_lflag &= ~(ICANON | ECHO);
    
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        perror("tcsetattr");
        return;
    }

    atexit(disableRawMode);
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings);
}