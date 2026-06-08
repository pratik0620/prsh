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

## Day 3 - 1 June 2026

### Learned

- How processes communicate using pipe
- How fork() and pipe() are used together
- How data flows from one process to another through a pipe

## Day 4 - 3 June 2026

### Learned

- One-way communication using pipes
- Two-way communication between processes

## Day 5 - 8 June 2026

### Progress

- Added pipe detection function
- Split pipe commands into left and right
- Tokenize both sides and build argv for both

### Learned

- A shell should always parse piped commands before execution

### Next Step

- Learn about file descriptors (stdin, stdout, stderr)
- Learn pipe()
- Learn dup2()
- Understand how two child processes communicate through a pipe
- Implement executePipe() for a single pipe
