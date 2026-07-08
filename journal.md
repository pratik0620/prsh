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

## Day 10 - 17 June 2026

### Progress

- Implemented Multiple pipes support

### Learned

- Communicating between multiple processes.
- When not use dup2 for stdout and stdin

#### How multiple pipes work

- all the commands are stored into a vector separated by '|'
- we require (N-1) pipes for executing N process.
- we create (N-1) pipes
- we create a vector to store the process_id
- we loop through it
- we fork process one by one
- for every child process, we change the stdout to current fd and the stdin to the previous fd.
- for first command, we don't change stdin and for last command we don't change stdout.
- after the loop, all the pipes are closed and data is cleaned.

## Day 11 - 19 June 2026

### Progress

- Implement Pipes with redirection
- Commands such as ls -a | grep cpp > out.txt now work correctly.
- Refactored command execution so that pipes and redirections can be handled together.

### Learned

- A shell works by splitting user input.
- First into commands by separating using (|).
- Each pipe runs it own fork() to create a child process.
- Pipes and redirections are not separate execution mechanisms. Both work by changing file descriptors using dup2().
- For pipeline execution, the shell creates all required pipes, forks child processes, connects their standard input/output streams and finally executes the command using execvp().
- Redirection (<, >, >>, <<) simply rewires a command's standard input or output before execution..

## Day 12 - 21 June 2026

### Progress

- Added CTRL+C signal handling (SIGINT)
- Added CTRL+D signal handling (eof)

### Learned

- sigaction() — allows a process to register custom behavior when a specific signal is received.
- sa_handler — specifies the function that should execute when a signal arrives.
- sa_flags — controls additional signal behavior such as SA_RESTART.
- SIGINT — generated when the user presses Ctrl+C in the terminal.
- sig_atomic_t — used to safely share state between a signal handler and the main program.
- Signal handlers should only use async-signal-safe functions such as write().

#### Notes

- The shell now survives Ctrl+C instead of terminating.
- Child processes still receive Ctrl+C and terminate normally.
- Ctrl+D exits the shell by detecting EOF from std::getline().

## Day 13 - 22 June 2026

### Progress

- Add background job
- Added zombie process cleanup using waitpid()
- Add environment variables (export, unset)

### Learned

- For background jobs, shell does nor call `waitpid()`
- Since `waitpid()` is not called, the kernel marks the process as zombie process after it finished
- `waitpid(-1, nullptr, WNOHANG)`, can be used to kill zombie process
- Background processes require periodic cleanup to prevent zombie processes from accumulating.
- `getenv()`, `setenv()`, `unsetenv()`
- regex

## Day 14 - 27 June 2026

### Progress

- Add support for raw mode

### Learned

- Learned about terminal modes - raw mode and cannonical mode
- In canonical mode, the terminal buffers user input and sends it to the program only after the Enter key is pressed
- In raw mode, the terminal delivers each key press to the program immediately instead of waiting for Enter.
- Since echo is disabled in raw mode, the program is responsible for displaying characters, handling backspace, arrow keys, and other editing features.
- `termios.h` provides APIs to configure terminal behavior.
- `termios.h` allows us to configure terminal attributes. Raw mode is achieved by modifying specific terminal flags (such as ICANON and ECHO) using tcgetattr() and tcsetattr().

## Day 14 - 28 June 2026

### Progress

- Add ARROW_KEY navigation
- History navigation using UP/DOWN arrow key
- cursor movement using LEFT/RIGHT arrow key
- Replaced `std::getline()` with a custom line editor using raw mode.

### Learned

- Arrow keys send ANSI escape sequences.
  - Up: `ESC [A`
  - Down: `ESC [B`
  - Right: `ESC [C`
  - Left: `ESC [D`
- In raw mode, input is read one byte at a time, these escape sequences must be parsed to identify special keys.
- `std::getline()` cannot support interactive editing because it only returns input after the Enter key is pressed. A custom line editor is required to process each key press immediately.
- A line editor maintains two pieces of state:
  - the input buffer, which stores the command being edited,
  - the cursor position, which tracks where the next character should be inserted or deleted.
- History navigation is implemented by maintaining a history index and replacing the current buffer with previous or next commands.
- The terminal display and the internal input buffer must always remain synchronized. After modifying the buffer (history navigation, insertion, deletion), the current line is redrawn to reflect the updated state.

## Day 15 - 02 July 2026

### Progress

- Optimized history performanc by loading history once at startup.

## Day 16 - 04 July 2026

### Progress

- Detected Ctrl+Z (SIGTSTP) for foreground processes.
- Added support for detecting stopped child processes using waitpid() and WUNTRACED.

### Learned

- Learned that pressing Ctrl+Z sends the SIGTSTP (Terminal Stop Signal) to the foreground process.
- Learned that child processes inherit the shell's signal handlers after fork(), making it necessary to restore the default signal behavior before calling execvp().
- `waitpid()` only waits for child process termination. When `WUNTRACED` flag is passed, it allows it to tell if a process is stopped.
- `WIFSTOPPED` tells is the process what suspended or terminated. (True->suspended, Fals->terminated).
- Learned that signal dispositions are inherited by child processes after fork(), so child processes must restore the default handlers (SIG_DFL) before executing a new program.

## Day 17 - 07 July 2026

### Progress

- Implemented job management subsystem.
- Added jobs builtin command to list suspended/background jobs.

### Learned

- how Unix shells maintain a job table to keep track of processes running in the foreground and background.
- why shells use a job ID separate from the operating system's PID, allowing users to refer to jobs using commands like fg, bg and jobs.

## Day 18 - 08 July 2026

### Progress

- Implemented fg and bg built-in commands.
- Added support for resuming suspended processes using SIGCONT.
- Updated job states based on process execution and completion.

### Learned

- Learned that fg (foreground) resumes a suspended process and transfers its execution back to the foreground, causing the shell to wait until the process exits or is suspended again.
- Learned that bg (background) resumes a suspended process without blocking the shell, allowing the user to continue executing other commands.
- Learned that the SIGCONT signal is used to resume a process that has been stopped by SIGTSTP (Ctrl+Z).
- how `waitpid()` is used in fg to wait for the resumed process, while bg resumes the process without waiting.
- `WIFSTOPPED(status)` checks whether the child process was suspended by a signal such as `SIGTSTP`
- `WIFEXITED(status)` checks whether the child process terminated normally, such as by returning from `main()` or calling `exit()`.
- `WIFSIGNALED(status)` checks whether the child process terminated due to a signal, such as `SIGINT (Ctrl+C)` or `SIGKILL`.

## Day 19 - 09 July 2026

### Progress

- Added Home, End, Del in line editor
- Refactored code