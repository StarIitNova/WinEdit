# WinEdit

A simple, C++ based utility to quickly add and remove extended window flags from windows on Windows.

This name is honestly probably taken, but I just wanted to make a quick tool for this.

Example usage:

`WinEdit.exe DesktopMate.exe +WS_EX_TOOLWINDOW -WS_EX_APPWINDOW`

All extended flags are supported (except the combination ones), and the prefix character (+ or -) decides whether the flag is added or removed from the process.

## Compiling

Quite simple, create a folder named "bin" and run make.

The build step does require g++, but you can change it in the makefile if you want (just set CXX to something else)
