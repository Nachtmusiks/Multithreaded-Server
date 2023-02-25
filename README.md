# Shannon Fano Elias Multithreading with Server and Client
This program perform Shannon Fano Elias encoding given a string input such as "AAABBBCCDD"\
Multithreading uses `pthread.h` from Linux not `<pthread>` from C++ library

# Build
Build using WSL and Visual Code \
Guide to WSL installation: https://github.com/programwithjohn/wsl-cpp-windows-installation \
Move the the given files from .vscode folder to the approriate location then use CTRL+R to build the program

# How to use
After building the program, open Visual Code and split the terminals like so \
![Visual Code Terminals](https://i.imgur.com/luomF9G.png)
First run the `./server [port#]` then run `./client localhost [port#]`
