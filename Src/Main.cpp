#include "Lexer.h"
#include "Parser.h"
#include "Scope.h"
#include "Token.h"
#include "Error.h"
#include "ScopeHandler.h"

#include <iostream>
#include <cstring>

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

    bool parsed = parser.Parse();
    bool codeGened = parser.OutputAssembly();

    if (debugToggle){
        std::cout << std::endl << "Parsed: " << parsed << std::endl;
        std::cout << std::endl << "Code Generated: " << codeGened << std::endl;
    }

    return !(parsed && codeGened);
}