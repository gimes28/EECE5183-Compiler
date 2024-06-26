C = gcc
CC = g++
CFLAGS = -std=c++11 -g

BUILDDIR = ../Build
SRC = $(wildcard Src/*.cpp)

LLVM = `llvm-config --cxxflags --ldflags --libs --system-libs`

Main: $(SRC) Runtime
	$(CC) $(CFLAGS) $(SRC) -o Main $(LLVM)

Runtime: Src/Runtime.c
	$(C) -c Src/Runtime.c -o runtime.o

parsetest: Main
	./testPgms/parsetest.sh

clean:
	rm Main runtime.o out.s