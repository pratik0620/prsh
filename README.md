# prsh - A Unix Shell built in C++

prsh is a Unix shell built from scratch in C++ as a learning project to understand how operating systems, processes and inter-process communication actually work under the hood. It implements core shell functionality including command execution, piping and I/O redirection using Linux system calls.

## Features

- REPL Loop
- Execute external commands using fork() and execvp()
- Built-in Commands
  - cd
  - clear
  - exit
- Multiple pipe (|) support
- Input redirection (<)
- Output redirection (>)
- Append redirection (>>)
- Here documents (<<)

## System Calls Used

- fork()
- execvp()
- waitpid()
- pipe()
- dup2()
- open()
- close()
- write()
- chdir()
- getcwd()

## Build and Run

```
git clone https://github.com/pratik0620/prsh.git
cd prsh
g++ ./main.cpp -o ./prsh.exe
./prsh.exe
```

## Why I Built This

I always wanted to build something from scratch on my own—something I use every day and that requires real technical depth to understand.

In my first year of college, the first language I learned was C++. I liked how explicit and controlled it felt, everything had a reason. Over time, though, my focus shifted toward web development, Node.js, APIs and databases. Everyone around me was building the same things - websites, AI wrappers, the same backends with different frontends.

Most of my previous projects were centered around web development and backend systems. While those projects taught me a lot, I wanted to understand what happens underneath the applications we use every day.

So I decided to go in a different direction. I have always been interested in operating systems and systems programming. Instead of building another web application, I chose to build something that would expose me directly to processes, system calls, file descriptors, and inter-process communication.

A shell felt like the perfect project because it sits at the boundary between users and the operating system. Building one from scratch forced me to learn how commands are executed, how processes communicate, and how shells manage input and output.

## What I Learned

#### Shell Parsing

A simple shell also requires to parse user input into commands, arguments, pipes and redirection. I learned what actually happens before a command is actually executed

#### Process creation

We execute a command by creating a new process using fork(). This new process is the child of the main process, this child process executes the command while the parent process waits for the child to complete it's execution.

#### Program execution

execvp() replaces the child process with a completely different program. This helped me understand why shells create a child process before executing commands.

#### File Descriptors

Standard input, output and errors are represented using file descriptors. Redirection works by manipulating these descriptors with dup2() before executing a command.

#### Pipes and Communication between processes

A pipe creates a path for communication between two processes. Where output of one command becomes the input of the other.

## Architecture

```
User Input
    |
    ▼
getCommandType()
    |
    |── Built-in ---> executeBuiltin()
    |
    |── Normal ---> executeExternal()
    |
    |── Pipe ---> executePipe()
    |
    |── Redirect ---> executeRedirect()
```

- `getCommandType()` — Takes the raw input string and detects what kind of command it is (pipe, redirection, or normal command) so the shell knows how to execute it.

- `trim()` — Removes leading and trailing whitespace from a string. Used after splitting commands to clean up tokens before execution.

- `tokenize()` — Takes user input and splits it into an array of string tokens separated by spaces.

- `buildArgs()` — Converts std::string tokens into a null-terminated C-style char\* array that execvp() requires.

- `buildCommandArgs()` — Combines tokenize() and buildArgs() into a single helper to reduce repeated parsing logic across the codebase.

- `executeExternal()` — Takes parsed args and executes an external program by forking a child process and replacing it with the target program using execvp(). Parent waits for the child to finish.

- `executeBuiltin()` — Handles built-in commands (cd, clear, exit) that must run in the shell process itself rather than a child process.

- `splitCommand()` — Splits any command string into left and right parts around an operator (|, >, >>, <, <<).

- `executePipe()` — Creates a kernel pipe to connect two processes. Forks two children, wiring the stdout of the left command to the stdin of the right command using dup2().

- `executeRedirect()` — Opens a file and rewires stdin or stdout to that file using dup2() before executing the command, implementing > , >>, and < in a single shared function.

- `main()` — The REPL loop. Reads user input, determines command type via getCommandType(), and dispatches to the correct execute function via a switch statement.

## Current Known Limitations

- Supports only a single pipe.
- No command history.
- No tab completion.
- No environment variable expansion.
- No job control or background processes.
- Limited support for quoted strings.
- Basic parser without advanced shell syntax.

## Future Plans

- Multiple pipe support
- Signal handling (Ctrl+C, Ctrl+Z)
- Command history
- Background jobs
