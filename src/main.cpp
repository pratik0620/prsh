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
#include "history.h"

int main() {
    handle_signal();
    while(true) {

        pid_t pid;
        while((pid = waitpid(-1, nullptr, WNOHANG)) > 0) {
            std::cout << "[PID " << pid << "]" << "\t" << "finished" << std::endl; 
        }

        char buffer[MAX_PATH_LENGTH];
        if(getcwd(buffer, sizeof(buffer)) == nullptr) {
            perror("getcwd");
            continue;
        }

        if(g_interrupted) {
            g_interrupted = 0;
            continue;
        }

        std::cout << "prsh " << buffer << "> " << std::flush;
        std::string user_input;
        if (!std::getline(std::cin, user_input)) {
            std::cout << "\nexit" << std::endl;
            sleep(1);
            break;
        }
        if(user_input.empty()) continue;

        if(!user_input.empty() && user_input[0] == '!') {
            std::string expanded = expandHistory(user_input);

            if(expanded.empty()) {
                std::cerr << "history: event not found\n";
                continue;
            }

            std::cout << expanded << "\n";
            user_input = expanded;
        }

        appendHistory(user_input);

        bool background_process = parseBackgroundProcess(user_input);

        if(user_input.empty() || user_input.find('&') != std::string::npos) {
            std::cerr << "syntax error near unexpected token '&'\n";
            continue;
        }

        std::vector<std::string> commands = splitPipeCommand(user_input);

        if(commands.size() == 1) {
            std::string tokens[MAX_ARGS];
            char *args[MAX_ARGS];

            int token_count = buildCommandArgs(commands[0], tokens, args);

            if(!executeBuiltin(args)) {
                executePipe(commands, background_process);
            }

            cleanup(args, token_count);
        } else {
            executePipe(commands, background_process);
        }
    }

    std::cout << std::endl;

    return 0;
}