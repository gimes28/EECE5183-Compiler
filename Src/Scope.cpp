#include "Scope.h"
#include "Symbol.h"
#include <iostream>

Scope::Scope(){
    prevScope = nullptr;
}

void Scope::SetSymbol(std::string str, Symbol sym){
    table[str] = sym;
}

Symbol Scope::GetSymbol(std::string str){
    if(HasSymbol(str))
        return table[str];
    return Symbol();
}   

bool Scope::HasSymbol(std::string str){
    return table.find(str) != table.end();
}

void Scope::PrintSymbolTable(){
    for(SymbolTable::iterator it = table.begin(); it != table.end(); ++it){
        std::cout << GetTokenTypeName(it->second.tt) << " ";
        std::cout << it->second.id << std::endl;
    }
    std::cout << std::endl;
}