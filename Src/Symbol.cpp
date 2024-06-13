#include "Symbol.h"
#include "Token.h"

Symbol::Symbol(){
    this->id = "";
    this->tt = T_UNK;
    this->st = ST_UNK;
    this->type = TYPE_UNK;
    this->isArr = false;
    this->arrSize = 0;
    this->isGlobal = false;
    this->isIndexed = false;
}

Symbol::Symbol(TokenType tt, std::string id){
    this->id = id;
    this->tt = tt;
    this->st = ST_UNK;
    this->type = TYPE_UNK;
    this->isArr = false;
    this->arrSize = 0;
    this->isGlobal = false;
    this->isIndexed = false;
}


Symbol::Symbol(TokenType tt, std::string id, SymbolType st, Type type){
    this->id = id;
    this->tt = tt;
    this->st = st;
    this->type = type;
    this->isArr = false;
    this->arrSize = 0;
    this->isGlobal = false;
    this->isIndexed = false;
}

std::string GetTypeName(Type typ){
    switch(typ){
        case(TYPE_INT):
            return "Integer";
        case(TYPE_FLOAT):
            return "Float";
        case(TYPE_BOOL):
            return "Boolean";
        case(TYPE_STRING):
            return "String";
        default:
            return "Unknown";
    }
}