#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "Token.h"

class Parser {
    public:
        Parser(Lexer* lexerPtr);
        ~Parser();
        void SetDebugOption(bool debug);
        bool Parse();
    private:
        Token tok;
        Lexer* lexer;
        bool debugOption = false;
        bool IsTokenType(TokenType tok);
        bool Program();
        bool ProgramHeader();
        bool ProgramBody();
        bool Identifier();
        bool Declaration();
        bool DeclarationAssist();
        bool Statement();
        bool StatementAssist();
        bool ProcedureDeclaration();
        bool VariableDeclaration();
        bool ProcedureHeader();
        bool ProcedureBody();
        bool TypeMark();
        bool ParameterList();
        bool Parameter();
        bool Bound();
        bool Number();
        bool AssignmentStatement();
        bool IfStatement();
        bool LoopStatement();
        bool ReturnStatement();
        bool ProcedureCall();
        bool ProcedureCallAssist();
        bool ArgumentList();
        bool Destination();
        bool Expression();
        bool ExpressionPrime();
        bool ArithOp();
        bool ArithOpPrime();
        bool Relation();
        bool RelationPrime();
        bool Term();
        bool TermPrime();
        bool Factor();
        bool Name();
        bool String();
};

#endif
