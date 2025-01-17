CXX=g++

OUT=bin/WinEdit

CFLAGS=-std=c++17 -O3
LFLAGS=

$(OUT): main.cpp
	$(CXX) $(CFLAGS) $^ -o $@ $(LFLAGS)
