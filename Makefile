CC = g++
CFLAGS = -std=c++11 -g

BUILDDIR = ../Build
SRC = $(wildcard Src/*.cpp)

LLVM = `llvm-config --cxxflags --ldflags --libs --system-libs`

Main: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o Main $(LLVM)

parsetest: Main
	./testPgms/parsetest.sh

clean:
	rm Main