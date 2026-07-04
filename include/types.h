#ifndef TYPES_H
#define TYPES_H

#include<string>

enum class CommandType {
    NORMAL,
    PIPE,
    OUTPUT_REDIRECT,
    APPEND_REDIRECT,
    INPUT_REDIRECT,
    HERE_DOC
};

enum class RedirectType {
    OUTPUT,
    APPEND,
    INPUT
};

enum class Key {
    CHARACTER,
    ENTER,
    BACKSPACE,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_RIGHT,
    ARROW_LEFT
};

enum class ReadStatus {
    SUCCESS,
    INTERRUPTED,
    END_OF_FILE
};

struct ReadResult {
    ReadStatus status;
    std::string line;
};

#endif