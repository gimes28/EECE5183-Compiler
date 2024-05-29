#ifndef SYMBOL_H
#define SYMBOL_H

#include "Token.h"

// Symbols stored in the symbol table are identifiers or reserved words

enum Type = {
    TYPE_BOOL,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_UNK
};

enum SymbolType = {
    ST_KEYWORD,
    ST_VARIABLE,
    ST_PROCEDURE,
    ST_UNK
}

class Symbol{
    public:
        Symbol();
        Symbol(std::string id, TokenType tt);
        Symbol(std::string id, TokenType tt, SymbolType st, Type type);
        void SetId(std::string id);
        std::string GetId();
        void SetTokenType(TokenType tt);
        TokenType GetTokenType();
        void SetSymbolType(SymbolType st);
        SymbolType GetSymbolType();
        void SetType(Type type);
        Type GetType();
        bool IsArray();
        void SetIsArray(bool val);
        int GetArrayBound();
        void SetArrayBound(int bound);
    private:
        std::string id;
        TokenType tt;
        SymbolType st;
        Type type;
        bool isArr;
        int arrBound;
};



#endif