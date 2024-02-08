#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <fstream>
#include <unordered_map>

#include "Token.h"

enum CharClass {
    NUM,
    LOWER_ALPHA,
    UPPER_ALPHA,
    SPACE,
    SPECIAL
};

class Lexer{
    public:
        Lexer();
        ~Lexer();
        bool LoadFile(std::string fileName);
        Token ScanToken();
        
        // Scan for individual tokens: ( ) { } etc...
        // Scan for white space: // /**/ " " etc...
    private:
        std::ifstream file;
        std::unordered_map<char, CharClass> charClass;
        int getCharClass(char t);

        int scanToken();        
        int lineNum;
};

#endif