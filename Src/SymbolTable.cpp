#include "SymbolTable.h"
#include "Token.h"

SymbolTable::SymbolTable(){
    symbolTable["program"] = (Token){T_PROGRAM, "program"};
    symbolTable["is"] = (Token){T_IS, "is"};
    symbolTable["begin"] = (Token){T_BEGIN, "begin"};
    symbolTable["end"] = (Token){T_END, "end"};
    symbolTable["global"] = (Token){T_GLOBAL, "global"};
    symbolTable["procedure"] = (Token){T_PROCEDURE, "procedure"};
    symbolTable["variable"] = (Token){T_VARIABLE, "variable"};
    symbolTable["integer"] = (Token){T_INTEGER, "integer"};
    symbolTable["double"] = (Token){T_DOUBLE, "double"};
    symbolTable["string"] = (Token){T_STRING, "string"};
    symbolTable["bool"] = (Token){T_BOOL, "bool"};
    symbolTable["if"] = (Token){T_IF, "if"};
    symbolTable["then"] = (Token){T_THEN, "then"};
    symbolTable["else"] = (Token){T_ELSE, "else"};
    symbolTable["for"] = (Token){T_FOR, "for"};
    symbolTable["return"] = (Token){T_RETURN, "return"};
    symbolTable["not"] = (Token){T_NOT, "not"};
    symbolTable["true"] = (Token){T_TRUE, "true"};
    symbolTable["false"] = (Token){T_FALSE, "false"};
}

void SymbolTable::setSymbol(std::string str, Token tok){
    symbolTable[str] = tok;
}

Token SymbolTable::getSymbol(std::string str){
    return symbolTable[str];
}

bool SymbolTable::hasSymbol(std::string str){
    return symbolTable.find(str) != symbolTable.end();
}

std::unordered_map<std::string, Token>::iterator SymbolTable::begin(){
    return symbolTable.begin();
}

std::unordered_map<std::string, Token>::iterator SymbolTable::end(){
    return symbolTable.end();
}