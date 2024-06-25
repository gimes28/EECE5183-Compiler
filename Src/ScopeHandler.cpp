#include "ScopeHandler.h"
#include "Scope.h"
#include "Symbol.h"

#include <iostream>

ScopeHandler::ScopeHandler() {
    global = new Scope();
    local = global;

    global->SetSymbol("program", Symbol(T_PROGRAM, "program", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("is", Symbol(T_IS, "is", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("begin", Symbol(T_BEGIN, "begin", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("end", Symbol(T_END, "end", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("global", Symbol(T_GLOBAL, "global", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("procedure", Symbol(T_PROCEDURE, "procedure", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("variable", Symbol(T_VARIABLE, "variable", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("integer", Symbol(T_INTEGER, "integer", ST_KEYWORD, TYPE_INT));
    global->SetSymbol("float", Symbol(T_FLOAT, "float", ST_KEYWORD, TYPE_FLOAT));
    global->SetSymbol("string", Symbol(T_STRING, "string", ST_KEYWORD, TYPE_STRING));
    global->SetSymbol("bool", Symbol(T_BOOL, "bool", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("if", Symbol(T_IF, "if", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("then", Symbol(T_THEN, "then", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("else", Symbol(T_ELSE, "else", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("for", Symbol(T_FOR, "for", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("return", Symbol(T_RETURN, "return", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("not", Symbol(T_NOT, "not", ST_KEYWORD, TYPE_UNK));
    global->SetSymbol("true", Symbol(T_TRUE, "true", ST_KEYWORD, TYPE_BOOL));
    global->SetSymbol("false", Symbol(T_FALSE, "false", ST_KEYWORD, TYPE_BOOL));

    global->SetSymbol("getbool", Symbol(T_IDENTIFIER, "getbool", ST_PROCEDURE, TYPE_BOOL));
    global->SetSymbol("getinteger", Symbol(T_IDENTIFIER, "getinteger", ST_PROCEDURE, TYPE_INT));
    global->SetSymbol("getfloat", Symbol(T_IDENTIFIER, "getfloat", ST_PROCEDURE, TYPE_FLOAT));
    global->SetSymbol("getstring", Symbol(T_IDENTIFIER, "getstring", ST_PROCEDURE, TYPE_STRING));

    Symbol sym = Symbol(T_IDENTIFIER, "putbool", ST_PROCEDURE, TYPE_BOOL);
    sym.params.push_back(Symbol(T_IDENTIFIER, "value", ST_VARIABLE, TYPE_BOOL));
    global->SetSymbol("putbool", sym);

    sym = Symbol(T_IDENTIFIER, "putinteger", ST_PROCEDURE, TYPE_BOOL);
    sym.params.push_back(Symbol(T_IDENTIFIER, "value", ST_VARIABLE, TYPE_INT));
    global->SetSymbol("putinteger", sym);

    sym = Symbol(T_IDENTIFIER, "putfloat", ST_PROCEDURE, TYPE_BOOL);
    sym.params.push_back(Symbol(T_IDENTIFIER, "value", ST_VARIABLE, TYPE_FLOAT));
    global->SetSymbol("putfloat", sym);

    sym = Symbol(T_IDENTIFIER, "putstring", ST_PROCEDURE, TYPE_BOOL);
    sym.params.push_back(Symbol(T_IDENTIFIER, "value", ST_VARIABLE, TYPE_STRING));
    global->SetSymbol("putstring", sym);

    sym = Symbol(T_IDENTIFIER, "sqrt", ST_PROCEDURE, TYPE_FLOAT);
    sym.params.push_back(Symbol(T_IDENTIFIER, "value", ST_VARIABLE, TYPE_INT));
    global->SetSymbol("sqrt", sym);
}

ScopeHandler::~ScopeHandler() {
    while (global != local) {
        ExitScope();
    }
    delete global;
}

void ScopeHandler::SetDebugOption(bool debug){
    debugOption = debug;
}

void ScopeHandler::NewScope() {
    Scope* tempScope = new Scope();
    tempScope->prevScope = local;
    local = tempScope;
}

void ScopeHandler::ExitScope() {
    if (debugOption) {
        PrintScope(false);
        std::cout << "Scope exited" << std::endl << std::endl;
    }

    if (local != nullptr && local != global) {
        Scope* tempScope = local;
        local = local->prevScope;
        delete tempScope;
    }
}

void ScopeHandler::SetSymbol(std::string str, Symbol sym, bool glob) {
    if (glob)
        global->SetSymbol(str, sym);
    else
        local->SetSymbol(str, sym);
}

Symbol ScopeHandler::GetSymbol(std::string str, bool glob) {
    if (glob)
        return global->GetSymbol(str);
    return local->GetSymbol(str);
}

Symbol ScopeHandler::GetSymbol(std::string str) {
    if(local->HasSymbol(str))
        return local->GetSymbol(str);
    return global->GetSymbol(str);
}

bool ScopeHandler::HasSymbol(std::string str, bool glob) {
    if (glob)
        return global->HasSymbol(str);
    return local->HasSymbol(str);
}

bool ScopeHandler::HasSymbol(std::string str) {
    return local->HasSymbol(str) || global->HasSymbol(str);
}

// Set current procedure symbol in  local scope 
void ScopeHandler::SetCurrentProcedure(Symbol proc){
    local->SetSymbol(CUR_PROC, proc);
}

// Find symbol for current procedure
Symbol ScopeHandler::GetCurrentProcedure(){
    return local->GetSymbol(CUR_PROC);
}

void ScopeHandler::PrintScope(bool glob) {
    if (glob)
        global->PrintSymbolTable();
    else
        local->PrintSymbolTable();
}

SymbolTable::iterator ScopeHandler::GetScopeBegin(bool glob){
    if (glob)
        return global->begin();
    return local->begin();
}

SymbolTable::iterator ScopeHandler::GetScopeEnd(bool glob){
    if (glob)
        return global->end();
    return local->end();
}