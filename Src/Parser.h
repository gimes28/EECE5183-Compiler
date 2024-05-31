#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "Token.h"
#include "ScopeHandler.h"
#include "Symbol.h"

class Parser {
    public:
        Parser(Lexer* lexerPtr, ScopeHandler* scoperPtr);
        ~Parser();
        void SetDebugOption(bool debug);
        bool Parse();
    private:
        Token tok;
        Lexer* lexer;
        ScopeHandler* scoper;
        bool debugOption = false;
        bool IsTokenType(TokenType tok);
        bool Program();
        bool ProgramHeader();
        bool ProgramBody();
        bool Identifier(Symbol &id);
        bool Declaration();
        bool DeclarationAssist();
        bool Statement();
        bool StatementAssist();
        bool ProcedureDeclaration(Symbol &decl);
        bool VariableDeclaration(Symbol &decl);
        bool ProcedureHeader(Symbol &decl);
        bool ProcedureBody();
        bool TypeMark();
        bool ParameterList(Symbol &decl);
        bool Parameter(Symbol &param);
        bool Bound();
        bool Number();
        bool AssignmentStatement();
        bool IfStatement();
        bool LoopStatement();
        bool ReturnStatement();
        bool ProcedureCall(Symbol &id);
        bool ProcedureCallAssist(Symbol &id);
        bool ArgumentList();
        bool Destination(Symbol &id);
        bool Expression();
        bool ExpressionPrime();
        bool ArithOp();
        bool ArithOpPrime();
        bool Relation();
        bool RelationPrime();
        bool Term();
        bool TermPrime();
        bool Factor();
        bool Name(Symbol &id);
        bool String();
};

#endif
