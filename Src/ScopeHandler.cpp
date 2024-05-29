#include "ScopeHandler.h"
#include "Scope.h"
#include "Symbol.h"

ScopeHandler::ScopeHandler() {
    global = new Scope();
    local = global;

    global->SetSymbol("program", Symbol(T_PROGRAM, "program"));
    global->SetSymbol("is", Symbol(T_IS, "is"));
    global->SetSymbol("begin", Symbol(T_BEGIN, "begin"));
    global->SetSymbol("end", Symbol(T_END, "end"));
    global->SetSymbol("global", Symbol(T_GLOBAL, "global"));
    global->SetSymbol("procedure", Symbol(T_PROCEDURE, "procedure"));
    global->SetSymbol("variable", Symbol(T_VARIABLE, "variable"));
    global->SetSymbol("integer", Symbol(T_INTEGER, "integer"));
    global->SetSymbol("float", Symbol(T_FLOAT, "float"));
    global->SetSymbol("string", Symbol(T_STRING, "string"));
    global->SetSymbol("bool", Symbol(T_BOOL, "bool"));
    global->SetSymbol("if", Symbol(T_IF, "if"));
    global->SetSymbol("then", Symbol(T_THEN, "then"));
    global->SetSymbol("else", Symbol(T_ELSE, "else"));
    global->SetSymbol("for", Symbol(T_FOR, "for"));
    global->SetSymbol("return", Symbol(T_RETURN, "return"));
    global->SetSymbol("not", Symbol(T_NOT, "not"));
    global->SetSymbol("true", Symbol(T_TRUE, "true"));
    global->SetSymbol("false", Symbol(T_FALSE, "false"));
}

ScopeHandler::~ScopeHandler() {
    if (global != local) {
        delete global;
    }
    delete local;
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

// Set symbol for procedure at procedure's scope and one above
void ScopeHandler::SetProcSymbol(std::string str, Symbol sym, bool glob) {
    // Add local within procedure, or globally
    SetSymbol(str, sym, glob);

    if (!glob) {
        Scope* ptr = local->prevScope;
        if (ptr != nullptr) {
            if (!ptr->HasSymbol(str))
                ptr->SetSymbol(str, sym);
        }
    }
}

Symbol ScopeHandler::GetSymbol(std::string str, bool glob) {
    if (glob)
        return global->GetSymbol(str);
    return local->GetSymbol(str);
}

bool ScopeHandler::HasSymbol(std::string str, bool glob) {
    if (glob)
        return global->HasSymbol(str);
    return local->HasSymbol(str);
}

void ScopeHandler::PrintScope(bool glob) {
    if (glob)
        global->PrintSymbolTable();
    else
        local->PrintSymbolTable();
}