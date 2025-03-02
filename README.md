# Custom Shell - Lightweight Command Line Shell
A simple custom shell implementation in C++ that supports built-in commands, aliasing, command history, background execution, and external commands using execvp.

Features
Supports external commands (execvp)
Built-in commands: cd, exit, help, pwd, history, alias, unalias
Aliases support (e.g., alias ll "ls -la")
Command history (history to view past commands)
Background execution (& at the end of a command)
Handles cd .. and cd ~ correctly
