#ifndef SCOPEHANDLER_H
#define SCOPEHANDLER_H

#include "Scope.h"
#include "Symbol.h"

class ScopeHandler{
    public:
        ScopeHandler();
        ~ScopeHandler();
        void SetDebugOption(bool debug);
        void NewScope();
        void ExitScope();
        void SetSymbol(std::string str, Symbol sym, bool glob);
        Symbol GetSymbol(std::string str, bool glob);
        Symbol GetSymbol(std::string str);
        bool HasSymbol(std::string str, bool glob);
        bool HasSymbol(std::string str);
        void PrintScope(bool glob);
    private:
        Scope* global;
        Scope* local;
        bool debugOption = false;
};

#endif