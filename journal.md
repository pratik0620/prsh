# prsh Development Journal

## Day 1 - 30 May 2026

### Progress

- Started the prsh shell project
- Implemented REPL
- Reads input
- Tokenized inputs
- Converted parsed tokens into a C-style argument array (char\* argv[])
- Added memory cleanup

### Learned

- What is REPL (Read, Evaluate, Print, Loop)
- Tokenization
- How std::string::c_str() converts a C++ string into a C-style string

## Day 2 - 31 May 2026

### Progress

- External Commands can run
- Implemented built in exit and cd command
- Added current directory to shell prompt
- Process creation using fork() and program execution using execvp()

## Learned

- fork(), wait(), execvp(), parent process, child process
- how chdir() and getcwd() from unistd.h is used
