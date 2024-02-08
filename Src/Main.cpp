#include "Lexer.h"
#include "Token.h"
#include "Error.h"

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
        tok = lexer.ScanToken();
        std::cout << lexer.getLineNumber() << ": " << tok.tt << " \n";
    }
    //Error::PrintErrorList();
    return 0;
}