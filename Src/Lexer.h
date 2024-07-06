#ifndef LEXER_H
#define LEXER_H

#include "ScopeHandler.h"
#include "Token.h"

#include <fstream>
#include <string>
#include <unordered_map>

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
        Lexer(ScopeHandler* scoperPtr);
        ~Lexer();
        bool LoadFile(std::string fileName);
        Token InitScan();
        Token ScanToken();
        int GetCharClass(char t);
        int GetLineNumber();
        std::string GetFileName(); 
        void SetDebugOption(bool opt);
        bool GetDebugOption();
        void Debug(Token tok);
    private:
        ScopeHandler* scoper;
        std::string fileName;
        std::ifstream file;
        std::unordered_map<char, CharClass> charClass;    
        int lineCount = 0;
        bool debugOption = false;
        bool error = false;
};

#endif