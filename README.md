# prsh - A Unix Shell built in C++

prsh is a Unix shell built from scratch in C++ as a learning project to understand how operating systems, processes and inter-process communication actually work under the hood. It implements core shell functionality including command execution, piping and I/O redirection using Linux system calls.

## Features

- REPL Loop
- Execute external commands using fork() and execvp()
- Multiple pipe support (`ls | grep .cpp | wc -l`)
- I/O Redirection: `>`, `>>`, `<`, `<<`
- Pipes combined with redirection (`ls | grep cpp > out.txt`)
- Background process execution (&)
- Command history (`!!`, `!n`, `!-n`) persisted to ~/.prsh_history
- Environment variable expansion ($VAR)
- Built-in commands: `cd`, `clear`, `exit`, `history`, `export`, `unset`, `fg`, `bg`, `jobs`
- Arrow key navigation (↑↓ history, ← → cursor movement)
- Home, End, Delete key support
- Custom line editor using POSIX `termios` (no GNU Readline dependency)
- Basic signal handling for SIGINT (Ctrl+C) and SIGTSTP (Ctrl+Z)

## System Calls Used

fork(), execvp(), waitpid(), pipe(), dup2(), open(), close(), write(), chdir(), getcwd(), sigaction(), tcgetattr(), tcsetattr(), read(), kill()

## Build and Run

```
git clone https://github.com/pratik0620/prsh.git
cd prsh
g++ src/*.cpp -Iinclude -o prsh
./prsh
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

**Raw Terminal Mode** — Terminals operate in cooked mode by default — the OS buffers input and handles keys like backspace. Switching to raw mode via termios gives the shell direct access to every keystroke, making arrow keys and cursor movement possible without any external library.

**Escape Sequences** — Arrow keys, Home, End and Delete are not single characters. They send multi-byte escape sequences (e.g. `\033[A` for up arrow). The shell must read and parse these sequences to respond correctly.

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
├── line_editor.cpp
├── terminal.cpp
├── jobs.cpp
└── utils.cpp
```

**Key functions:**

- `getCommandType()` — detects pipe, redirect, or normal command
- `splitPipeCommand()` — splits input into N commands on |
- `executePipe()` — creates N-1 pipes, forks N children, wires fds
- `executeRedirect()` — opens file, rewires fd with dup2, returns command
- `expandVariables()` — replaces $VAR with environment values
- `expandHistory()` — resolves !!, !n, !-n from history file
- `readKey()` — parses raw keystrokes and escape sequences
- `readLine()` — line editor loop with history and cursor support
- `refreshLine()` — redraws current input line using terminal escapes

## Current Known Limitations

- No tab completion
- Single level of redirection per command
- No quoted string support
- No POSIX process groups or terminal control (`setpgid`, `tcsetpgrp`)
- Basic parser without full POSIX shell syntax

## Changelog

- [v3.0.0](docs/v3.0.0.md)
- [v2.0.0](docs/v2.0.0.md)
- [v1.0.0](docs/v1.0.0.md)

## Future Plans

- Tab completion
- POSIX process groups and full terminal job control
- Quoted string support
