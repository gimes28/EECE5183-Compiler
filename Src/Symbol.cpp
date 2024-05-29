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

Symbol::Symbol(TokenType tt, std::string id){
    this->id = id;
    this->tt = tt;
    this->st = ST_UNK;
    this->type = TYPE_UNK;
    this->isArr = false;
    this->arrBound = 0;
}


Symbol::Symbol(TokenType tt, std::string id, SymbolType st, Type type){
    this->id = id;
    this->tt = tt;
    this->st = st;
    this->type = type;
    this->isArr = false;
    this->arrBound = 0;
}