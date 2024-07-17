### Compiler
This compiler is designed with respect to the specifications given in class EECE5183/EECE6083

I decided to use C++ as it's my most proficient language but decided to use Ubuntu Linux for development as I've had very little experience programming and debugging with the Linux OS.  

## Requirements
* gcc and g++
* c++17
* llvm-config version 14.0.0
* make

### How to use
```
Usage: ./Main [file] [options]
Options:
  -h | -help       ->   Help
  -d | -debug-all  ->   Show debug for all
  -debug-lexer     ->   Show debug for lexer
  -debug-parser    ->   Show debug for parser
  -debug-symtab    ->   Show symbol table after exiting each scope
  -debug-codegen   ->   Output LLVM IR in out.ll file
```

## Build
Build the compiler
- `make`

## Normal use
Run the compiler and generate an executable file
-`./Main.sh <file> <options>`

Then run the executable using
-`./a.out`

## Alternative use
Only run the compiler, which generates the output assembly file 
-`./Main <file> <options>`

## Clean
Remove compiled solution (remove runtime.o and out.s files) 
- `make clean`

### Testing
The `testPgms` contains two folders for correct and incorrect source code files. 
You can run all source code files in the correct folder using `make Tests`. This folder includes test cases from the orignal compressed file downloaded from the class homepage as well as other test cases that were created during development.

The incorrect folder includes test cases from the orignal compressed file downloaded from the class homepage as well as two other test cases that highlight basic resync capabilities.

### What's next?
* Fix memory leaks
* Find a solution for the outofboundserror to be in the correct error format(link with Error file)
* Enhance the resync point implementation
* Refactor lexer for more modular/cleaner format
* Tracking variable initalization




