#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "Token.h"
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, Token> SymbolTableMap;

class SymbolTable {
    public:
        SymbolTable();
        void setSymbol(std::string str, Token tok);
        Token getSymbol(std::string str);
        bool hasSymbol(std::string str);
        SymbolTableMap::iterator begin();
        SymbolTableMap::iterator end();
    private:
        SymbolTableMap symbolTable;
};

extern SymbolTable symTable;

#endif