#include <iostream>
#include "Lexer.h"
#include "Token.h"

int main(int argc, char* args[])
{
    std::cout << "Hello World\n";

    if (argc < 2) 
       std::cout << "Less than 2 argc" << std::endl;

    Lexer lexer;
    if(!lexer.LoadFile(args[1])) 
        std::cout << "File not opened: " << args[1] << std::endl;
    else
        std::cout << "File Loaded: "<< args[1] << std::endl;

    Token token = Token();
    while(token.tt != T_EOF){
        token = lexer.ScanToken();
        std::cout << token.tt << std::endl;
    }
    return 0;
}