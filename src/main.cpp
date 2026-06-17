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

void executeExternal(char* args[]) {
    pid_t pid = fork();

    if(pid == 0) {
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("Fork Failed");
        return;
    }
}

int main() {
    while(true) {
        char buffer[MAX_PATH_LENGTH];
        if(getcwd(buffer, sizeof(buffer)) == nullptr) {
            perror("getcwd");
            continue;
        }

        std::cout << "prsh " << buffer << ">" << std::flush;
        std::string user_input;
        if (!std::getline(std::cin, user_input)) {
            break;
        }
        if(user_input.empty()) continue;

        CommandType command_type = getCommandType(user_input);

        switch (command_type) {
        case CommandType::PIPE: {
            std::vector<std::string> pipe_commands = splitPipeCommand(user_input);
            executePipe(pipe_commands);
            break;
        }

        case CommandType::OUTPUT_REDIRECT:
            executeRedirect(user_input, RedirectType::OUTPUT, O_WRONLY | O_CREAT | O_TRUNC, STDOUT_FILENO);
            break;

        case CommandType::APPEND_REDIRECT:
            executeRedirect(user_input, RedirectType::APPEND, O_WRONLY | O_CREAT | O_APPEND, STDOUT_FILENO);
            break;

        case CommandType::INPUT_REDIRECT:
            executeRedirect(user_input, RedirectType::INPUT, O_RDONLY, STDIN_FILENO);
            break;

        case CommandType::HERE_DOC:
            executeHereDoc(user_input, CommandType::HERE_DOC);
            break;

        case CommandType::NORMAL: {
            std::string tokens[MAX_ARGS];            
            char *args[MAX_ARGS];
            int token_count = buildCommandArgs(user_input, tokens, args);

            if(!executeBuiltin(args)) executeExternal(args);
            cleanup(args, token_count);
            
            break;
        }

        default:
            break;
        }
    }

    std::cout << std::endl;

    return 0;
}