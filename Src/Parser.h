#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "ScopeHandler.h"
#include "Symbol.h"
#include "Token.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

class Parser {
    public:
        Parser(Lexer* lexerPtr, ScopeHandler* scoperPtr);
        ~Parser();
        void SetDebugOptionParser(bool debug);
        void SetDebugOptionCodeGen(bool debug);
        void DebugParseTrace(std::string message);
        bool Parse();

        bool OutputAssembly();
    private:
        Token tok;
        Lexer* lexer;
        ScopeHandler* scoper;
        bool debugOptionParser = false;
        bool debugOptionCodeGen = false;
        bool listError = false;

        bool IsTokenType(TokenType tok);
        bool Program();
        bool ProgramHeader();
        bool ProgramBody();
        bool Identifier(Symbol &id);
        bool Declaration();
        bool Statement();
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
        bool ArgumentList(Symbol &id,  std::vector<llvm::Value*> &argList);
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
        
        bool DeclarationAssist();
        bool StatementAssist();
        bool ProcedureCallOrName(Symbol &id);
        bool ArrayIndexAssist(Symbol &id);

        bool ArithmeticTypeCheck(Symbol &lhs, Symbol &rhs, Token &op);
        bool RelationTypeCheck(Symbol &lhs, Symbol &rhs, Token &tok);
        bool ExpressionTypeCheck(Symbol &lhs, Symbol &rhs, Token &op);
        bool CompatibleTypeCheck(Symbol &dest, Symbol &exp);

        llvm::Value* StringCompare(Symbol& lhs, Symbol& rhs);
        llvm::Type* GetLLVMType(Type t);

        llvm::LLVMContext *llvmContext;
        llvm::IRBuilder<> *llvmBuilder;   
        llvm::Module *llvmModule;     
};

#endif
