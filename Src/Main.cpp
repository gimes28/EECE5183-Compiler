#include "Lexer.h"
#include "Parser.h"
#include "Token.h"
#include "Error.h"
#include "SymbolTable.h"

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

    Lexer lexer;
    lexer.SetDebugOption(debugToggle);

    if(!lexer.LoadFile(args[1])) 
        std::cout << "File not opened: " << args[1] << std::endl;
    else
        std::cout << "File Loaded: "<< args[1] << std::endl;

    Parser parser(&lexer);
    parser.SetDebugOption(debugToggle);

    //Token tok = Token();
    /*while(tok.tt != T_EOF){
        tok = lexer.InitScan();
        //std::cout << lexer.getLineNumber() << ": " << tok.tt << " \n";
        //lexer.Debug(tok);
    }*/
    
    bool parsed = parser.Parse();
    std::cout << parsed << std::endl;
    
    std::cout << "Symbol Table: " << std::endl;
    for(SymbolTableMap::iterator it = symTable.begin(); it != symTable.end(); ++it){
        std::cout << getTokenTypeName(it->second) << " ";
        switch(it->second.tt) {
        case(T_INTEGER_CONST):
            std::cout << it->second.val.intVal << std::endl;
            break;
        case(T_FLOAT_CONST):
            std::cout << it->second.val.floatVal << std::endl;
            break;
        default:
            std::cout << it->second.val.stringVal << std::endl;
            break;
        }
    }
    return 0;
}