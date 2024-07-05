C = gcc
CC = g++
CFLAGS = -std=c++17 -g

BUILDDIR = ../Build
SRC = $(wildcard Src/*.cpp)

LLVM = `llvm-config --cxxflags --ldflags --libs --system-libs`

Main: $(SRC) Runtime
	$(CC) $(CFLAGS) $(SRC) -o Main $(LLVM) -no-pie

Runtime: Src/Runtime.c
	$(C) -c -no-pie Src/Runtime.c -o runtime.o

RunTest: Main
	./testPgms/tests.sh

clean:
	rm Main runtime.o out.s