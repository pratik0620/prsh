#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "parser.h"
#include "pipe.h"
#include "redirect.h"
#include "builtin.h"
#include "utils.h"
#include "constants.h"
#include "signal_handler.h"

int main() {
    handle_signal();
    while(true) {
        char buffer[MAX_PATH_LENGTH];
        if(getcwd(buffer, sizeof(buffer)) == nullptr) {
            perror("getcwd");
            continue;
        }

        if(g_interrupted) {
            g_interrupted = 0;
            continue;
        }

        std::cout << "prsh " << buffer << ">" << std::flush;
        std::string user_input;
        if (!std::getline(std::cin, user_input)) {
            std::cout << "exit" << std::endl;
            break;
        }
        if(user_input.empty()) continue;

        std::vector<std::string> commands = splitPipeCommand(user_input);

        if(commands.size() == 1) {
            std::string tokens[MAX_ARGS];
            char *args[MAX_ARGS];

            int token_count = buildCommandArgs(commands[0], tokens, args);

            if(!executeBuiltin(args)) {
                executePipe(commands);
            }

            cleanup(args, token_count);
        } else {
            executePipe(commands);
        }
    }

    std::cout << std::endl;

    return 0;
}