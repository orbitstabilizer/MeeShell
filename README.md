# MeeShell
Pronounced as "michelle", a reference to The Beatles' song [Michelle](https://www.youtube.com/watch?v=WoBLi5eE-wY).

## Build & Execution
```bash
make # Build
./meeshell # Run
```
## Features

### Tokenization

- Following are special tokens: `>`, `>>`, `>>>`, `&`. The input is split into tokens by spaces, special tokens, or quoted strings.
- When a special token is encountered it is added to the token list as a separate token, if it is not quoted.
- `&` token must be the last token in the input, otherwise an error is thrown.

### Prompt

- Prompt replaces `~` with the home directory of the user.
- Input is read to a buffer of size 1024.

### Aliases

- Aliases are stored in `.meeshrc` file. 
- At the beginning of the program, .meeshrc file is read and aliases are stored in a hash map, see `dictionary.h` for implementation.
- Aliases can be added with `alias key=value` syntax, and removed with `alias key=""` syntax.
- After an expression is matched with above syntax the rest of the line is ignored.
- Aliases can be overwritten.
- Aliases are expanded once, no recursive expansion.
- All the session aliases are written to .meeshrc file at the end of the program.
### bello

- Current Shell holds the name of parent process, if it is not a shell (for example logind, then its value is logind). Alternatives: `$SHELL` environment variable stores the login shell, which may not be the parent shell.
- Number of processes shows the number of running processes spawned by the shell, except the dummy processes created for `>>>`.
- `bello` output can be redirected and run in background. 

### cd

- `cd` is a built-in command, changes the current working directory.

### exit

- `exit` is a built-in command, exits the shell. `<Ctrl-D>` also exits the shell.
- `<Ctrl-C>` does not exit the shell, prints a new line.

## Command Execution

- All external commands are executed with `fork()` and `execv()`. Commands are first searched in the aliases, if not found, then in the PATH environment variable, it is assumed that it is an absolute/relative path.
- Background processes are kept in `user->bg_pids` list. Number of processes in `bello` is the length of this list. When a background process terminates, it is removed from the list, and reaped with `waitpid()` in a signal handler. 

### Redirection
- `>` and `>>` redirections are handled by `dup2()` calls.
- `>>>` redirection in background is handled by creating a dummy process, which waits for the command to finish, and then exits. Dummy processes are not included in `bello` process count. Dummy process reads from a pipe, reverses the input, and writes to the file.

## Implementation
The project is implemented in OOP style:
Main:

- `main.c` contains the main loop, and the signal handlers.

Classes:

- `tokenizer.c` contains the tokenizer implementation.
- `dictionary.c` contains the hash map implementation.
- `repl.c` contains the REPL implementation.
- `user.c` contains the user object implementation.

Utilities:
 
- `utils.c` contains the utility functions, for executing commands, redirections, and user info.
Please refer to header files for documentation.














