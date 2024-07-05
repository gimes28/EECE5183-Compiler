#!/bin/sh
./Main "$@"
g++ -no-pie out.s runtime.o -o a.out -lm