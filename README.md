# Simple Shell Command Project

## Introduction
A simple shell command in C using the command line interface. The shell itself can change directories and execute system programs such as `pwd` and `ls`. The project involves high usage of memory allocation with different header and driver files to implement a complete shell in C.


## Project Structure
Four files are included in this project:
* `Makefile`- contains information used to compile your program with the `make` command.
* `shell.h`- includes declarations and specifications for all of the functions in `shell.c`. 
* `shell.c`- contains function definitions for all functions in `shell.c`. In particular, your goal for this assignment will be to implement the following functions: `parse`, `find_fullpath`, and `execute`.
* `driver.c`- contains the main function, which is just a loop that reads in a command and uses the functions written in `shell.c` to determine whether the command is valid and handle it appropriately. Reading this file before you begin will help you understand the usage of functions you must implement.
