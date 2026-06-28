#ifndef TYPES_H
#define TYPES_H

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

#endif