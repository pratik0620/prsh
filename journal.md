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

## Day 6 - 10 June 2026

### Progress

- Implemented executePipe()
- Added support for single-command pipelines
- Connected stdout of one process to stdin of another using dup2()
- Added error handling for pipe(), fork(), execvp(), and dup2()

### Learned

#### File Descriptors (stdin - 0, stdout - 1, stderr - 2)

- 0 (stdin) – Standard Input (usually keyboard)
- 1 (stdout) – Standard Output (usually terminal screen)
- 2 (stderr) – Standard Error (used for error messages)
- Commands read from stdin and write to stdout by default.

#### Pipe

A pipe is an IPC (Inter-Process Communication) mechanism that allows data to flow from one process to another.

- pipe() creates a unidirectional communication channel between processes.

- A pipe has two ends:\
  fd[0] ---> Read End\
  fd[1] ---> Write End\

- Data written to fd[1] can be read from fd[0].

#### dup2()

- dup2(oldfd, newfd) redirects a file descriptor.
- Used to replace stdin or stdout with a pipe endpoint.
- It connects a process stdin/stdout to pipe endpoints.

#### How a shell pipeline works

1. Create a pipe
2. Fork first child
3. Redirect stdout to pipe write end
4. Execute left command
5. Fork second child
6. Redirect stdin to pipe read end
7. Execute right command
8. Parent waits for both children

## Day 7 - 12 June 2026

### Progress

- Implemented output redirection (>)
- Implemented append redirection (>>)
- Added file creation using open()
- Redirected stdout to files using dup2()
- Added error handling for open() and dup2()
- O_WRONLY, O_CREAT, O_APPEND, O_TRUNC

### Learned

- Used to open or create files.
- Returns a file descriptor.

## Day 8 - 13 June 2026

### Progress

- Implemented input redirection (<)
- Implemented here document redirection (<<)

### Learned

- Redirecting file data as stdin using fd and dup2

## Day 9 - 14 June 2026

### Progress

- Refactored command parsing using CommandType enum
- Refactored redirection handling using RedirectType enum
- Added shared splitCommand() helper for pipes and redirections
- Added buildCommandArgs() helper to reduce duplicated parsing logic
- Simplified main() by replacing multiple condition checks with a switch-based command dispatcher
