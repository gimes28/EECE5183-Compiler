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
        bool TypeMark(Symbol &id);
        bool ParameterList(Symbol &decl);
        bool Parameter(Symbol &param);
        bool Bound(Symbol &id);
        bool Number(Symbol &num);
        bool AssignmentStatement();
        bool IfStatement();
        bool LoopStatement();
        bool ReturnStatement();
        bool ProcedureCallAssist(Symbol &id);
        bool ArgumentList();
        bool Destination(Symbol &id);
        bool Expression(Symbol &exp);
        bool ExpressionPrime(Symbol &exp);
        bool ArithOp(Symbol &aro);
        bool ArithOpPrime(Symbol &aro);
        bool Relation(Symbol &rel);
        bool RelationPrime(Symbol &rel);
        bool Term(Symbol &trm);
        bool TermPrime(Symbol &trm);
        bool Factor(Symbol &fac);
        bool Name(Symbol &id);
        bool String(Symbol &str);
        bool NameAssist(Symbol &id);
};

#endif
