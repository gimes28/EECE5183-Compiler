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
    bool debugToggle = false;

    if (argc < 2){ 
        std::cout << "Error: Missing argument" << std::endl;
        return 1;
    }
    else if (argc <= 3){
        for(int i = 2; i < argc; i++){
            if(!strcmp(args[i], "-d"))
                debugToggle = true;
        }
    }
    else{
        std::cout << "Error: Too many arguments" << std:: endl;
        return 1;    
    }

    ScopeHandler scoper;
    Lexer lexer(&scoper);
    Parser parser(&lexer, &scoper);

    scoper.SetDebugOption(false);
    parser.SetDebugOption(debugToggle);
    lexer.SetDebugOption(debugToggle);  

    if(!lexer.LoadFile(args[1])) 
        return 1;

    bool success = parser.Parse();

    if (debugToggle){
        std::cout << std::endl << "Parsed: " << success << std::endl;
        if(success){
            success = parser.OutputAssembly();
            std::cout << "Code Generated: " << success << std::endl;
        }            
    }

    return !success;
}