#include "Symbol.h"
#include "Token.h"

Symbol::Symbol(){
    this->id = "";
    this->tt = T_UNK;
    this->st = ST_UNK;
    this->type = TYPE_UNK;
    this->isArr = false;
    this->arrBound = 0;
}

Symbol::Symbol(std::string id, TokenType tt){
    this->id = id;
    this->tt = tt;
    this->st = ST_UNK;
    this->type = TYPE_UNK;
    this->isArr = false;
    this->arrBound = 0;
}


Symbol::Symbol(std::string id, TokenType tt, SymbolType st, Type type){
    this->id = id;
    this->tt = tt;
    this->st = st;
    this->type = type;
    this->isArr = false;
    this->arrBound = 0;
}

void Symbol::SetId(std::string id){
    this->id = id;
}

std::string Symbol::GetId(){
    return id;
}

void Symbol::SetTokenType(TokenType tt){
    this->tt = tt;
}

TokenType Symbol::GetTokenType(){
    return tt;
}

void Symbol::SetSymbolType(SymbolType st){
    this->st = st;
}

SymbolType Symbol::GetSymbolType(){
    return st;
}

void Symbol::SetType(Type type){
    this->type = type;
}

Type Symbol::GetType(){
    return type;
}

bool Symbol::IsArray() { 
    return isArr; 
}

void Symbol::SetIsArray(bool isArr) { 
    this->isArr = isArr; 
}

int Symbol::GetArrayBound() { 
    return arrBound; 
}

void Symbol::SetArrayBound(int arrBound) { 
    this->arrBound = arrBound; 
}