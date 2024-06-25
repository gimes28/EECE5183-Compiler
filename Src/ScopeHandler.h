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

        SymbolTable::iterator GetScopeBegin(bool glob = false);
        SymbolTable::iterator GetScopeEnd(bool glob = false);

        void SetCurrentProcedure(Symbol proc);
        Symbol GetCurrentProcedure();
        void PrintScope(bool glob);
    private:
        Scope* global;
        Scope* local;
        bool debugOption = false;

        // Created to find the current procedure symbol in the local scope
        std::string CUR_PROC = "CUR_PROC";
};

#endif