#ifndef SYMBOL_H
#define SYMBOL_H

#include "Token.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

#include <vector>

// Symbols stored in the symbol table are identifiers or reserved words

// Type of variable or return type of procedure

enum Type {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_UNK
};

// Symbols are either reserved words or identifiers

enum SymbolType {
    ST_KEYWORD,
    ST_VARIABLE,
    ST_PROCEDURE,
    ST_UNK
};

struct Symbol{
    Symbol();
    Symbol(TokenType tt, std::string id);
    Symbol(TokenType tt, std::string id, SymbolType st, Type type);
    std::string id;
    TokenType tt;
    SymbolType st;
    Type type;
    bool isArr;
    int arrSize;
    bool isGlobal;
    bool isIndexed;

    std::vector<Symbol> params;

    llvm::Value *llvmValue;
    llvm::Function *llvmFunction;
};

std::string GetTypeName(Type typ);

#endif