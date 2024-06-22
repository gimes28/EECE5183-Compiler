#include "Error.h"
#include "Lexer.h"
#include "Parser.h"
#include "Scope.h"
#include "ScopeHandler.h"
#include "Token.h"

#include <cstring>
#include <iostream>

int main(int argc, char* args[])
{
    bool debugLexer = false;
    bool debugParser = false;
    bool debugSymTab = false;
    bool debugCodeGen = false;

    if (argc < 2){ 
        std::cout << "Error: Missing filename argument" << std::endl;

        return 1;
    }
    else if (argc <= 6){
        for(int i = 1; i < argc; i++){
            if(!strcmp(args[i], "-h") || !strcmp(args[i], "-h")){
                std::cout << "Usage: ./Main [file] [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  -h | --help      ->   Help" << std::endl;
                std::cout << "  -d | --debug-all ->   Show debug for all" << std::endl;
                std::cout << "  --debug-lexer    ->   Show debug for lexer" << std::endl;
                std::cout << "  --debug-parser   ->   Show debug for parser" << std::endl;
                std::cout << "  --debug-symtab   ->   Show symbol table after exiting each scope" << std::endl;
                std::cout << "  --debug-codegen  ->   Output LLVM IR in out.ll file" << std::endl;
                return 0;
            }
            else if(!strcmp(args[i], "-d") || !strcmp(args[i], "-debug-all")){
                debugLexer = true;
                debugParser = true;
                debugSymTab = true;
                debugCodeGen = true;
            }
            else if(!strcmp(args[i], "--debug-lexer")){
                debugLexer = true;
            }
            else if(!strcmp(args[i], "--debug-parser")){
                debugParser = true;
            }
            else if(!strcmp(args[i], "--debug-symtab")){
                debugSymTab = true;
            }
            else if(!strcmp(args[i], "--debug-codegen")){
                debugCodeGen = true;
            }
        }
    }
    else{
        std::cout << "Error: Too many arguments" << std:: endl;
        return 1;    
    }

    ScopeHandler scoper;
    Lexer lexer(&scoper);
    Parser parser(&lexer, &scoper);

    scoper.SetDebugOption(debugSymTab);
    lexer.SetDebugOption(debugLexer); 
    parser.SetDebugOptionParser(debugParser); 
    parser.SetDebugOptionCodeGen(debugCodeGen); 

    if(!lexer.LoadFile(args[1])){ 
        std::cout << "Error: File could not be opened" << std::endl;
        return 1;
    }
    bool success = parser.Parse() && parser.OutputAssembly();

    return !success;
}