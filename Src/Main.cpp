#include "Lexer.h"
#include "Parser.h"
#include "Token.h"
#include "Error.h"
#include "SymbolTable.h"

#include <iostream>

int main(int argc, char* args[])
{
    if (argc < 2) 
       std::cout << "Less than 2 argc" << std::endl;

    Lexer lexer;
    if(!lexer.LoadFile(args[1])) 
        std::cout << "File not opened: " << args[1] << std::endl;
    else
        std::cout << "File Loaded: "<< args[1] << std::endl;

    lexer.SetDebugOption(true);
    
    Token tok = Token();
    while(tok.tt != T_EOF){
        tok = lexer.InitScan();
        //std::cout << lexer.getLineNumber() << ": " << tok.tt << " \n";
    }
    /*Parser par(&lexer);

    bool parsed = par.Parse();
    std::cout << "Parsed? " << parsed << std::endl;*/

    /*
    std::cout << "Symbol Table: " << std::endl;
    for(SymbolTableMap::iterator it = symTable.begin(); it != symTable.end(); ++it){
        std::cout << getTokenTypeName(it->second) << " " << std::endl;
        switch(it->second.tt) {
        case(T_INTEGER_CONST):
            std::cout << it->second.val.intVal << std::endl;
            break;
        case(T_DOUBLE_CONST):
            std::cout << it->second.val.doubleVal << std::endl;
            break;
        case(T_STRING_CONST || T_IDENTIFIER):
            std::cout << it->second.val.stringVal << std::endl;
            break;
        }
    }*/
    //Error::PrintErrorList();
    return 0;
}