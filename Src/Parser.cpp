#include "Parser.h"
#include "Lexer.h"
#include "Error.h"
#include "ScopeHandler.h"
#include "Symbol.h"

#include <iostream>

Parser::Parser(Lexer* lexerptr, ScopeHandler* scoperPtr){
    lexer = lexerptr;
    scoper = scoperPtr;
}

Parser::~Parser(){

}

bool Parser::IsTokenType(TokenType token){
    if(tok.tt == token){
        tok = lexer->InitScan();
        return true;
    }
    return false;
}

void Parser::SetDebugOption(bool debug){
    debugOption = debug;
}

bool Parser::Parse(){
    tok = lexer->InitScan();
    return Program();
}

bool Parser::Program(){
    //std::cout <<"Program" << //std::endl;
    //Global scope is defined in scopeHandler
    if (!ProgramHeader())
        return false;

    if (!ProgramBody())
        return false;

    if (!IsTokenType(T_PERIOD)){
        errTable.ReportError(ERROR_MISSING_PERIOD, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    if(!IsTokenType(T_EOF))
        return false;
    scoper->ExitScope();
    return true;
}

bool Parser::ProgramHeader(){
    //std::cout <<"ProgramHeader" << //std::endl;
    if (!IsTokenType(T_PROGRAM))
        return false;
    Symbol id;
    if (!Identifier(id)){
        errTable.ReportError(ERROR_INVALID_IDENTIFIER, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + id.id + "\'");
        return false;
    }

    if(!IsTokenType(T_IS)){
        errTable.ReportError(ERROR_INVALID_HEADER, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    return true;    
}

bool Parser::ProgramBody(){
    //std::cout <<"ProgramBody" << //std::endl;
    if (!DeclarationAssist())
        return false;

    if (!IsTokenType(T_BEGIN)){
        // ERROR
        //std::cout <<"ProgramBody1" << //std::endl;
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if (!StatementAssist())
        return false;
    
    if (!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if (!IsTokenType(T_PROGRAM)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    return true;
}

bool Parser::Declaration(){
    //std::cout <<"Declaration" << //std::endl;
    Symbol decl;
    decl.isGlobal = IsTokenType(T_GLOBAL);
    //std::cout <<"Decleration Started" << //std::endl;
    if (ProcedureDeclaration(decl)) {
        //std::cout <<"Proc Decleration Finished" << //std::endl;
    } else if (VariableDeclaration(decl)) {
        //std::cout <<"Var Decleration Finished" << //std::endl;
    }
    else
        return false;
    return true;
}

bool Parser::ProcedureDeclaration(Symbol &decl){
    //std::cout <<"ProcedureDeclaration" << //std::endl;
    if(!ProcedureHeader(decl))
        return false;

    decl.st = ST_PROCEDURE;

    if (scoper->HasSymbol(decl.id, decl.isGlobal)){
        errTable.ReportError(ERROR_DUPLICATE_IDENTIFIER, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + decl.id + "\'");
        return false;
    }
    
    if(!ProcedureBody())
        return false;
    scoper->ExitScope();

    if (!decl.isGlobal){
        if (scoper->HasSymbol(decl.id, decl.isGlobal)){
            errTable.ReportError(ERROR_DUPLICATE_PROCEDURE, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + decl.id + "\'");
            return false;
        }
    }
    scoper->SetSymbol(decl.id, decl, decl.isGlobal);

    return true;
}

bool Parser::ProcedureHeader(Symbol &decl){
    //std::cout <<"ProcedureHeader" << //std::endl;
    if(!IsTokenType(T_PROCEDURE))
        return false;

    scoper->NewScope();

    if (!Identifier(decl)){
        errTable.ReportError(ERROR_INVALID_IDENTIFIER, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + decl.id + "\'");
        return false;
    }

    if(!IsTokenType(T_COLON)){
        errTable.ReportError(ERROR_MISSING_COLON, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if(!TypeMark(decl)){
        errTable.ReportError(ERROR_INVALID_TYPE_MARK, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    
    if(!IsTokenType(T_LPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    ParameterList(decl);

    if(!IsTokenType(T_RPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    return true;
}

bool Parser::ParameterList(Symbol &decl){
    //std::cout <<"ParameterList" << //std::endl;
    Symbol param;
    if(!Parameter(param))
        return false;

    decl.params.push_back(param);

    while(IsTokenType(T_COMMA)){
        if (!Parameter(param)) {
            errTable.ReportError(ERROR_INVALID_PARAMETER, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
    }
    return true;
}

bool Parser::Parameter(Symbol &param){
    //std::cout <<"Parameter" << //std::endl;
    return VariableDeclaration(param);
}

bool Parser::ProcedureBody(){
    //std::cout <<"ProcedureBody" << //std::endl;
    if (!DeclarationAssist())
        return false;

    if (!IsTokenType(T_BEGIN)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if (!StatementAssist())
        return false;
    
    if (!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if (!IsTokenType(T_PROCEDURE)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    return true;
}

bool Parser::VariableDeclaration(Symbol &decl){
    //std::cout << "VariableDeclaration" << //std::endl;
    if (!IsTokenType(T_VARIABLE))
        return false;

    decl.st = ST_VARIABLE;

    if (!Identifier(decl)){
        errTable.ReportError(ERROR_DUPLICATE_VARIABLE, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + decl.id + "\'");
        return false;
    }

    if(!IsTokenType(T_COLON)){
        errTable.ReportError(ERROR_MISSING_COLON, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if(!TypeMark(decl)){
        errTable.ReportError(ERROR_INVALID_TYPE_MARK, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    
    if(IsTokenType(T_LBRACKET)){
        
        decl.isArr = true;

        if(!Bound(decl)){
            errTable.ReportError(ERROR_INVALID_BOUND, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
        if(!IsTokenType(T_RBRACKET)){
            errTable.ReportError(ERROR_MISSING_BRACKET, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
    }

    scoper->SetSymbol (decl.id, decl, decl.isGlobal);

    return true;
}

bool Parser::TypeMark(Symbol &id){
    //std::cout << "TypeMark" << //std::endl;
    if (IsTokenType(T_INTEGER))
        id.type = TYPE_INT;
    else if (IsTokenType(T_FLOAT))
        id.type = TYPE_FLOAT;
    else if (IsTokenType(T_STRING))
        id.type = TYPE_STRING;
    else if (IsTokenType(T_BOOL))
        id.type = TYPE_BOOL;
    else
        return false;
    return true;
}

bool Parser::Bound(Symbol &id){
    //std::cout << "Bound" << //std::endl;
    Symbol num;

    int val = tok.val.intVal;

    if(Number(num) && num.type == TYPE_INT){
        id.arrBound = val;
        return true;
    }
    else{
        errTable.ReportError(ERROR_INVALID_BOUND, lexer->GetFileName(), lexer->GetLineNumber(), "Bound must be an integer");
        return false;
    }
}

bool Parser::Statement(){
    //std::cout << "Statement" << //std::endl;
    if (AssignmentStatement()){}
    else if (IfStatement()){}
    else if (LoopStatement()){}
    else if (ReturnStatement()){}
    else 
        return false;
    return true;
}

bool Parser::AssignmentStatement(){
    //std::cout << "AssignmentStatement" << //std::endl;
    Symbol decl, exp;
    if(!Destination(decl))
        return false;

    if (!IsTokenType(T_ASSIGNMENT)){
        return false;
    }

    if (!Expression(exp))
        return false;
    

    // Perform type check

    return true;    
}

bool Parser::Destination(Symbol &id){
    //std::cout << "Destination" << //std::endl;
    if(!Identifier(id))
        return false;
    
    if(!scoper->HasSymbol(id.id)){
        errTable.ReportError(ERROR_SCOPE_DECLERATION, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + id.id + "\'");
        return false;
    }

    id = scoper->GetSymbol(id.id);

    if(IsTokenType(T_LBRACKET)){      
        Symbol exp;   
        if (!Expression(exp))
            return false;
        
        // Check access

        if (!IsTokenType(T_RBRACKET)){
            errTable.ReportError(ERROR_MISSING_BRACKET, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }   
    }
    return true; 
}

bool Parser::IfStatement(){
    //std::cout << "IfStatement" << //std::endl;
    if (!IsTokenType(T_IF)){
        return false;
    }

    if(!IsTokenType(T_LPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    Symbol exp;   
    if(!Expression(exp))
        return false;

    // Check and convert to bool

    if(!IsTokenType(T_RPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if(!IsTokenType(T_THEN)){
        errTable.ReportError(ERROR_INVALID_IF, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if(!StatementAssist())
        return false;

    if(IsTokenType(T_ELSE)){
        if(!StatementAssist())
            return false;
    }

    if(!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_IF, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if(!IsTokenType(T_IF)){
        errTable.ReportError(ERROR_INVALID_IF, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    return true; 
}

bool Parser::LoopStatement(){
    //std::cout << "LoopStatement" << //std::endl;
    if(!IsTokenType(T_FOR)){
        return false;
    }
     
    if (!IsTokenType(T_LPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber());
        return false;   
    }

    if(!AssignmentStatement())
        return false;

    if(!IsTokenType(T_SEMICOLON)){
        errTable.ReportError(ERROR_MISSING_SEMICOLON, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    Symbol exp;
    if(!Expression(exp))
        return false;

    // Check and convert to bool

    if(!IsTokenType(T_RPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if(!StatementAssist())
        return false;
    
    if(!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_LOOP, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    if(!IsTokenType(T_FOR)){
        errTable.ReportError(ERROR_INVALID_LOOP, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    return true;
}

bool Parser::ReturnStatement(){
    //std::cout << "ReturnStatement" << //std::endl;
    if (!IsTokenType(T_RETURN))
        return false;

    Symbol exp;
    if(!Expression(exp))
        return false;

    return true;
}

bool Parser::Identifier(Symbol &id){
    //std::cout <<"Identifier" << //std::endl;
    if (tok.tt == T_IDENTIFIER){
        id.id = tok.val.stringVal;
        id.tt = tok.tt;
    }
    return IsTokenType(T_IDENTIFIER);
}

bool Parser::Expression(Symbol &exp){
    //std::cout << "Expression" << //std::endl;
    bool notToken = IsTokenType(T_NOT);

    if(!ArithOp(exp))
        return false;

    if(notToken){
        if(exp.type != TYPE_BOOL && exp.type != TYPE_INT)
            errTable.ReportError(ERROR_INVALID_EXPRESSION, lexer->GetFileName(), lexer->GetLineNumber(), "\'not\' operator only defined for int and bool");
    }

    if(!ExpressionPrime(exp))
        return false;

    return true;
}

bool Parser::ExpressionPrime(Symbol &exp){
    //std::cout << "ExpressionPrime" << //std::endl;
    if (IsTokenType(T_AND) || IsTokenType(T_OR)){
        Symbol rhs;
        if(!ArithOp(rhs))
            return false;
        
        ExpressionTypeCheck(exp, rhs);

        if(!ExpressionPrime(exp))
            return false;
    }
    return true;
}

bool Parser::ArithOp(Symbol &aro){
    //std::cout << "ArithOp" << //std::endl;
    if(!Relation(aro))
        return false;

    if(!ArithOpPrime(aro))
        return false;
    
    return true;
}

bool Parser::ArithOpPrime(Symbol &aro){
    //std::cout << "ArithOpPrime" << //std::endl;
    if (IsTokenType(T_PLUS) || IsTokenType(T_MINUS)){
        Symbol rhs;
        if(!Relation(rhs))
            return false;
        
        if(!ArithmeticTypeCheck(aro, rhs))
            return false;

        if(!ArithOpPrime(aro))
            return false;
    }
    return true;
}

bool Parser::Relation(Symbol &rel){
    //std::cout << "Relation" << //std::endl;
    if(!Term(rel))
        return false;
    
    // Check and convert type for relation ops

    if(!RelationPrime(rel))
        return false;

    return true;
}

bool Parser::RelationPrime(Symbol &rel){
    //std::cout << "RelationPrime" << //std::endl;
    Token op = tok;

    if(IsTokenType(T_LESS) || IsTokenType(T_LESS_EQ) || 
        IsTokenType(T_GREATER) || IsTokenType(T_GREATER_EQ) || 
        IsTokenType(T_EQUAL) || IsTokenType(T_NOT_EQUAL)){

        Symbol rhs;    
        if(!Term(rhs))
            return false;

        // Check and convert type for relation ops
        if(!RelationTypeCheck(rel, rhs, op))
            return false;

        rel.type = TYPE_BOOL;

        if(!RelationPrime(rel))
            return false;
    }
    return true;
}

bool Parser::Term(Symbol &trm){
    //std::cout << "Term" << //std::endl;
    if(!Factor(trm))
        return false;
    
    if(!TermPrime(trm))
        return false;

    return true;
}

bool Parser::TermPrime(Symbol &trm){
    //std::cout << "TermPrime" << //std::endl;
    if(IsTokenType(T_MULTIPLY) || IsTokenType(T_DIVIDE)){
        Symbol rhs;
        if(!Factor(rhs))
            return false;
        // Check and convert for *

        if(!ArithmeticTypeCheck(trm, rhs))
            return false;

        if(!TermPrime(trm))
            return false;
    }
    return true;
}

bool Parser::Factor(Symbol &fac){
    //std::cout << "Factor" << //std::endl;
    if(IsTokenType(T_LPAREN)){
        if(!Expression(fac))
            return false;
        
        if(!IsTokenType(T_RPAREN)){
            errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
    }
    else if(ProcedureCallAssist(fac)){}
    else if(IsTokenType(T_MINUS)){
        if (Name(fac)){}
        else if(Number(fac)){}
        else{
            errTable.ReportError(ERROR_INVALID_CHARACTER, lexer->GetFileName(), lexer->GetLineNumber(), "Invalid use of \'-\'");
            return false;
        }
    }
    else if(Number(fac)){}
    else if(String(fac)){}
    else if(IsTokenType(T_TRUE)){
        fac.tt = T_TRUE;
        fac.type = TYPE_BOOL;
    }
    else if(IsTokenType(T_FALSE)){
        fac.tt = T_FALSE;
        fac.type = TYPE_BOOL;
    }
    else
        return false;
    return true;
}

bool Parser::Name(Symbol &id){
    //std::cout << "Name" << //std::endl;
    if(!Identifier(id))
        return false;
        
    if(!scoper->HasSymbol(id.id)){
        errTable.ReportError(ERROR_SCOPE_DECLERATION, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + id.id + "\'");
        return false;
    }

    id = scoper->GetSymbol(id.id);

    if (!NameAssist(id)){
        return false;
    }
    return true;
}

bool Parser::ArgumentList(){
    //std::cout << "ArgumentList" << //std::endl;
    Symbol exp;
    if(!Expression(exp))
        return false;

    // Check type match to param
    
    while(IsTokenType(T_COMMA)){
        exp = Symbol();
        if(!Expression(exp)){
            errTable.ReportError(ERROR_INVALID_ARGUMENT, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
        // Check type match to param
    }
    return true;
}

bool Parser::Number(Symbol &num){
    //std::cout << "Number" << //std::endl;

    if(tok.tt == T_INTEGER_CONST){
        num.type = TYPE_INT;
        num.tt = T_INTEGER_CONST;

        return IsTokenType(T_INTEGER_CONST);
    }
    else if(tok.tt == T_FLOAT_CONST){
        num.type = TYPE_FLOAT;
        num.tt = T_FLOAT_CONST;

        return IsTokenType(T_FLOAT_CONST);
    }
    else
        return false;
}

bool Parser::String(Symbol &str){
    //std::cout << "String" << //std::endl;
    if(tok.tt == T_STRING_CONST){
        str.id = tok.val.stringVal;
        str.tt = tok.tt;
        str.type = TYPE_STRING;
    }

    return IsTokenType(T_STRING_CONST);
}

bool Parser::DeclarationAssist(){
    while(Declaration()){
        if(!IsTokenType(T_SEMICOLON)){
            errTable.ReportError(ERROR_MISSING_SEMICOLON, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
    }
    return true;
}

bool Parser::StatementAssist(){
    while(Statement()){
        if(!IsTokenType(T_SEMICOLON)){
            errTable.ReportError(ERROR_MISSING_SEMICOLON, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
    }
    return true;
}


bool Parser::ProcedureCallAssist(Symbol &id){
    //std::cout << "ProcedureCallAssist" << //std::endl;
    if (!Identifier(id))
        return false;
    
    if(!scoper->HasSymbol(id.id)){
        errTable.ReportError(ERROR_SCOPE_DECLERATION, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + id.id + "\'");
        /*
            Return false later
        */
        //return false;
    }

    id = scoper->GetSymbol(id.id);

    if(IsTokenType(T_LPAREN)){
        ArgumentList();

        if (!IsTokenType(T_RPAREN)){
            errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
        return true;  
    }
    else{
        if(!NameAssist(id))
            return false;
    }
    return true;
}

bool Parser::NameAssist(Symbol &id){
    if (IsTokenType(T_LBRACKET)){
        Symbol exp;
        if(!Expression(exp))
            return false;
        
        // Check valid access

        if(!IsTokenType(T_RBRACKET)){
            errTable.ReportError(ERROR_MISSING_BRACKET, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
    }
    return true;
}

bool Parser::ArithmeticTypeCheck(Symbol &lhs, Symbol &rhs){
    if((lhs.type != TYPE_INT && lhs.type != TYPE_FLOAT) || (rhs.type != TYPE_INT && rhs.type != TYPE_FLOAT)){
        errTable.ReportError(ERROR_INVALID_TYPE_CHECK, lexer->GetFileName(), lexer->GetLineNumber(), "Arithmetic only defined for int and float");
        return false;
    }

    if(lhs.type == TYPE_INT){
        if(rhs.type == TYPE_FLOAT)
            // convert lhs to float 
            lhs.type = TYPE_FLOAT;
    }
    else{
        if(rhs.type == TYPE_INT)
            // convert rhs to float 
            rhs.type = TYPE_FLOAT;
    }
    return true;
}

bool Parser::RelationTypeCheck(Symbol &lhs, Symbol &rhs, Token &tok){
    // if int is present with float or bool, convert int to respective type

    bool comp = false;

    if(lhs.type == TYPE_INT) {
        if(rhs.type == TYPE_BOOL) {
            comp = true;
            lhs.type = TYPE_BOOL;
        }
        else if(rhs.type == TYPE_FLOAT) {
            comp = true;
            lhs.type = TYPE_FLOAT;
        }
        else if (rhs.type == TYPE_INT)
            comp = true;
    }
    else if(lhs.type == TYPE_FLOAT) {
        if(rhs.type == TYPE_FLOAT) 
            comp = true;
        else if(rhs.type == TYPE_INT) {
            comp = true;
            rhs.type = TYPE_FLOAT;
        }
    }
    else if(lhs.type == TYPE_BOOL) {
        if(rhs.type == TYPE_BOOL)
            comp = true;
        else if(rhs.type == TYPE_INT) {
            comp = true;
            rhs.type = TYPE_BOOL;
        }
    }
    else if(lhs.type == TYPE_STRING) {
        if(rhs.type == TYPE_STRING && (tok.tt == T_EQUAL || tok.tt == T_NOT_EQUAL))
            comp = true;
    }

    if(!comp)
        errTable.ReportError(ERROR_INVALID_RELATION, lexer->GetFileName(), lexer->GetLineNumber());
    
    return comp;
}

bool Parser::ExpressionTypeCheck(Symbol &lhs, Symbol &rhs){
    bool comp = false;

    if(lhs.type == TYPE_BOOL && rhs.type == TYPE_BOOL)
        comp = true;
    else if(lhs.type == TYPE_INT && rhs.type == TYPE_INT)
        comp = true;
    
    if(!comp)
        errTable.ReportError(ERROR_INVALID_EXPRESSION, lexer->GetFileName(), lexer->GetLineNumber(), "Expression operations only defined for int and bool");
    
    return comp;
}