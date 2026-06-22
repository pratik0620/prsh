# prsh - A Unix Shell built in C++

prsh is a Unix shell built from scratch in C++ as a learning project to understand how operating systems, processes and inter-process communication actually work under the hood. It implements core shell functionality including command execution, piping and I/O redirection using Linux system calls.

## Features

- REPL Loop
- Execute external commands using fork() and execvp()
- Multiple pipe support (ls | grep .cpp | wc -l)
- I/O Redirection: >, >>, <,
- Pipes combined with redirection (ls | grep cpp > out.txt)
- Background process execution (&)
- Command history (!!, !n, !-n) persisted to ~/.prsh_history
- Environment variable expansion ($VAR)
- Built-in commands: cd, clear, exit, history, export, unset

## System Calls Used

fork(), execvp(), waitpid(), pipe(), dup2(), open(), close(), write(), chdir(), getcwd(), sigaction()

## Build and Run

```
git clone https://github.com/pratik0620/prsh.git
cd prsh
g++ src/*.cpp -Iinclude -o main.exe
./prsh.exe
```

## Why I Built This

I always wanted to build something from scratch on my own—something I use every day and that requires real technical depth to understand.

In my first year of college, the first language I learned was C++. I liked how explicit and controlled it felt, everything had a reason. Over time, though, my focus shifted toward web development, Node.js, APIs and databases. Everyone around me was building the same things - websites, AI wrappers, the same backends with different frontends.

Most of my previous projects were centered around web development and backend systems. While those projects taught me a lot, I wanted to understand what happens underneath the applications we use every day.

So I decided to go in a different direction. I have always been interested in operating systems and systems programming. Instead of building another web application, I chose to build something that would expose me directly to processes, system calls, file descriptors, and inter-process communication.

A shell felt like the perfect project because it sits at the boundary between users and the operating system. Building one from scratch forced me to learn how commands are executed, how processes communicate, and how shells manage input and output.

## What I Learned

**Shell Parsing** - A simple shell also requires to parse user input into commands, arguments, pipes and redirection. I learned what actually happens before a command is actually executed

**Process creation** - We execute a command by creating a new process using fork(). This new process is the child of the main process, this child process executes the command while the parent process waits for the child to complete it's execution.

**Program execution** - execvp() replaces the child process with a completely different program. This helped me understand why shells create a child process before executing commands.

**File Descriptors** - Standard input, output and errors are represented using file descriptors. Redirection works by manipulating these descriptors with dup2() before executing a command.

**Pipes and Communication between processes** - A pipe is a kernel buffer. N commands connected by pipes need N-1 pipes. Each child's stdout is wired to the next child's stdin using dup2(). The key insight: if you don't close unused pipe ends, the reading process hangs forever waiting for EOF that never comes.

**Signal Handling — SIGINT (Ctrl+C)** - SIGINT (Ctrl+C) must kill the foreground child but not the shell itself. The shell installs a custom handler that sets a flag, child processes reset to default handler before exec so they die normally on Ctrl+C.

## Architecture

```
src/
├── main.cpp
├── parser.cpp
├── pipe.cpp
├── redirect.cpp
├── builtin.cpp
├── executor.cpp
├── signal_handler.cpp
├── history.cpp
└── utils.cpp
```

**Key functions:**

- `getCommandType()` — detects pipe, redirect, or normal command
- `splitPipeCommand()` — splits input into N commands on |
- `executePipe()` — creates N-1 pipes, forks N children, wires fds
- `executeRedirect()` — opens file, rewires fd with dup2, returns command
- `expandVariables()` — replaces $VAR with environment values
- `expandHistory()` — resolves !!, !n, !-n from history file

## Current Known Limitations

- No arrow key navigation (requires termios/readline)
- No tab completion
- Single level of redirection per command
- No quoted string support
- Basic parser without full POSIX shell syntax

## Changelog

- [v2.0.0](docs/v2.0.0.md)
- [v1.0.0](docs/v1.0.0.md)

## Future Plans

- Arrow key history navigation (termios)
- Tab completion
- Signal handling for Ctrl+Z (SIGTSTP)
