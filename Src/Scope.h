#ifndef SCOPE_H
#define SCOPE_H

#include "Symbol.h"

#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, Symbol> SymbolTable;

class Scope {
    public:
        Scope();
        void SetSymbol(std::string str, Symbol sym);
        Symbol GetSymbol(std::string str);
        bool HasSymbol(std::string str);
        void PrintSymbolTable();
        Scope* prevScope;
    private:
        SymbolTable table;
};

#endif