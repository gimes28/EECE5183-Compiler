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
    SPECIAL,
    SPACE,
    INVALID = -2
};

class Lexer{
    public:
        Lexer();
        ~Lexer();
        bool LoadFile(std::string fileName);
        Token InitScan();
        Token ScanToken();
        int getCharClass(char t);
        int getLineNumber();
        std::string getFileName(); 
        void SetDebugOption(bool opt);
        bool GetDebugOption();
        void Debug(Token tok);
        void PushSymbolTable(Token tok);
    private:
        std::string fileName;
        std::ifstream file;
        std::unordered_map<char, CharClass> charClass;    
        int lineCount = 0;
        bool debugOption = false;
};

#endif