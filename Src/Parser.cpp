#include "Parser.h"
#include "Lexer.h"
#include "Error.h"

#include <iostream>

Parser::Parser(Lexer* lexerptr){
    lexer = lexerptr;
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

bool Parser::Parse(){
    tok = lexer->InitScan();
    return Program();
}

bool Parser::Program(){
    //std::cout <<"Program" << //std::endl;
    if (!ProgramHeader())
        return false;

    if (!ProgramBody())
        return false;

    if (!IsTokenType(T_PERIOD)){
        errTable.ReportError(ERROR_MISSING_PERIOD);
        return false;
    }
    if(!IsTokenType(T_EOF))
        return false;
    return true;
}

bool Parser::ProgramHeader(){
    //std::cout <<"ProgramHeader" << //std::endl;
    if (!IsTokenType(T_PROGRAM))
        return false;

    if (!Identifier())
        return false;

    if(!IsTokenType(T_IS)){
        errTable.ReportError(ERROR_INVALID_HEADER);
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
        errTable.ReportError(ERROR_INVALID_BODY);
        return false;
    }

    if (!StatementAssist())
        return false;
    
    if (!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_BODY);
        return false;
    }

    if (!IsTokenType(T_PROGRAM)){
        errTable.ReportError(ERROR_INVALID_BODY);
        return false;
    }
    return true;
}

bool Parser::Declaration(){
    //std::cout <<"Declaration" << //std::endl;
    if (IsTokenType(T_GLOBAL)){
        //errTable.ReportError(ERROR_INVALID_DECLARATION);
        //return false;
    }
    //std::cout <<"Decleration Started" << //std::endl;
    if (ProcedureDeclaration()) {
        //std::cout <<"Proc Decleration Finished" << //std::endl;
    } else if (VariableDeclaration()) {
        //std::cout <<"Var Decleration Finished" << //std::endl;
    }
    else
        return false;
    return true;
}

bool Parser::ProcedureDeclaration(){
    //std::cout <<"ProcedureDeclaration" << //std::endl;
    if(!ProcedureHeader())
        return false;
    if(!ProcedureBody())
        return false;
    return true;
}

bool Parser::ProcedureHeader(){
    //std::cout <<"ProcedureHeader" << //std::endl;
    if(!IsTokenType(T_PROCEDURE))
        return false;

    if (!Identifier())
        return false;

    if(!IsTokenType(T_COLON)){
        errTable.ReportError(ERROR_MISSING_COLON);
        return false;
    }

    if(!TypeMark())
        return false;
    
    if(!IsTokenType(T_LPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN);
        return false;
    }

    ParameterList();

    if(!IsTokenType(T_RPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN);
        return false;
    }
    return true;
}

bool Parser::ParameterList(){
    //std::cout <<"ParameterList" << //std::endl;
    if(!Parameter())
        return false;

    if(IsTokenType(T_COMMA)){
        ParameterList();
    }
    return true;
}

bool Parser::Parameter(){
    //std::cout <<"Parameter" << //std::endl;
    return VariableDeclaration();
}

bool Parser::ProcedureBody(){
    //std::cout <<"ProcedureBody" << //std::endl;
    if (!DeclarationAssist())
        return false;

    if (!IsTokenType(T_BEGIN)){
        errTable.ReportError(ERROR_INVALID_BODY);
        return false;
    }

    if (!StatementAssist())
        return false;
    
    if (!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_BODY);
        return false;
    }

    if (!IsTokenType(T_PROCEDURE)){
        errTable.ReportError(ERROR_INVALID_BODY);
        return false;
    }
    return true;
}

bool Parser::VariableDeclaration(){
    //std::cout << "VariableDeclaration" << //std::endl;
    if (!IsTokenType(T_VARIABLE))
        return false;

    if (!Identifier())
        return false;
    
    if(!IsTokenType(T_COLON)){
        errTable.ReportError(ERROR_MISSING_COLON);
        return false;
    }

    if(!TypeMark())
        return false;
    
    if(IsTokenType(T_LBRACKET)){
        if(!Bound())
            return false;

        if(!IsTokenType(T_RBRACKET)){
            errTable.ReportError(ERROR_MISSING_BRACKET);
            return false;
        }
    }
    return true;
}

bool Parser::TypeMark(){
    //std::cout << "TypeMark" << //std::endl;
    if(IsTokenType(T_INTEGER) || IsTokenType(T_FLOAT) || 
        IsTokenType(T_STRING) || IsTokenType(T_BOOL))
        return true;
    return false;
}

bool Parser::Bound(){
    //std::cout << "Bound" << //std::endl;
    return Number();
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

bool Parser::ProcedureCall(){
    //std::cout << "ProcedureCall" << //std::endl;
    if(!Identifier())
        return false;

    if(!IsTokenType(T_LPAREN))
        return false;
    
    ArgumentList();

    if (!IsTokenType(T_RPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN);
        return false;
    }
    return true;    
}

bool Parser::AssignmentStatement(){
    //std::cout << "AssignmentStatement" << //std::endl;
    if(!Destination())
        return false;

    if (!IsTokenType(T_ASSIGNMENT)){
        return false;
    }

    if (!Expression())
        return false;
    
    return true;    
}

bool Parser::Destination(){
    //std::cout << "Destination" << //std::endl;
    if(!Identifier())
        return false;
    
    if(IsTokenType(T_LBRACKET)){         
        if (!Expression())
            return false;
        
        if (!IsTokenType(T_RPAREN)){
            errTable.ReportError(ERROR_MISSING_PAREN);
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
        errTable.ReportError(ERROR_MISSING_PAREN);
        return false;
    }

    if(!Expression())
        return false;

    if(!IsTokenType(T_RPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN);
        return false;
    }

    if(!IsTokenType(T_THEN)){
        errTable.ReportError(ERROR_INVALID_IF);
        return false;
    }

    if(!StatementAssist())
        return false;

    if(IsTokenType(T_ELSE)){
        if(!StatementAssist())
            return false;
    }

    if(!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_IF);
        return false;
    }

    if(!IsTokenType(T_IF)){
        errTable.ReportError(ERROR_INVALID_IF);
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
        errTable.ReportError(ERROR_MISSING_PAREN);
        return false;   
    }

    if(!AssignmentStatement())
        return false;

    if(!IsTokenType(T_SEMICOLON)){
        errTable.ReportError(ERROR_MISSING_SEMICOLON);
        return false;
    }

    if(!Expression())
        return false;

    if(!IsTokenType(T_RPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN);
        return false;
    }

    if(!StatementAssist())
        return false;
    
    if(!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_LOOP);
        return false;
    }

    if(!IsTokenType(T_FOR)){
        errTable.ReportError(ERROR_INVALID_LOOP);
        return false;
    }
    return true;
}

bool Parser::ReturnStatement(){
    //std::cout << "ReturnStatement" << //std::endl;
    if (!IsTokenType(T_RETURN))
        return false;

    if(!Expression())
        return false;

    return true;
}

bool Parser::Identifier(){
    //std::cout <<"Identifier" << //std::endl;
    return (IsTokenType(T_IDENTIFIER));
}

bool Parser::Expression(){
    //std::cout << "Expression" << //std::endl;
    if(IsTokenType(T_NOT)){

    }
    if(!ArithOp())
        return false;

    if(!ExpressionPrime())
        return false;

    return true;
}

bool Parser::ExpressionPrime(){
    //std::cout << "ExpressionPrime" << //std::endl;
    if (IsTokenType(T_AND) || IsTokenType(T_OR)){
        if(!ArithOp())
            return false;
        
        if(!ExpressionPrime())
            return false;
    }
    return true;
}

bool Parser::ArithOp(){
    //std::cout << "ArithOp" << //std::endl;
    if(!Relation())
        return false;
    
    if(!ArithOpPrime())
        return false;
    
    return true;
}

bool Parser::ArithOpPrime(){
    //std::cout << "ArithOpPrime" << //std::endl;
    if (IsTokenType(T_PLUS) || IsTokenType(T_MINUS)){
        if(!Relation())
            return false;
        
        if(!ArithOpPrime())
            return false;
    }
    return true;
}

bool Parser::Relation(){
    //std::cout << "Relation" << //std::endl;
    if(!Term())
        return false;
    
    if(!RelationPrime())
        return false;

    return true;
}

bool Parser::RelationPrime(){
    //std::cout << "RelationPrime" << //std::endl;
    if(IsTokenType(T_LESS) || IsTokenType(T_LESS_EQ) || 
        IsTokenType(T_GREATER) || IsTokenType(T_GREATER_EQ) || 
        IsTokenType(T_EQUAL) || IsTokenType(T_NOT_EQUAL)){
        if(!Term())
            return false;
        if(!RelationPrime())
            return false;
    }
    return true;
}

bool Parser::Term(){
    //std::cout << "Term" << //std::endl;
    if(!Factor())
        return false;
    
    if(!TermPrime())
        return false;

    return true;
}

bool Parser::TermPrime(){
    //std::cout << "TermPrime" << //std::endl;
    if(IsTokenType(T_MULTIPLY) || IsTokenType(T_DIVIDE)){
        if(!Factor())
            return false;

        if(!TermPrime())
            return false;
    }
    return true;
}

bool Parser::Factor(){
    //std::cout << "Factor" << //std::endl;
    if(IsTokenType(T_LPAREN)){
        if(!Expression())
            return false;
        
        if(!IsTokenType(T_RPAREN)){
            errTable.ReportError(ERROR_MISSING_PAREN);
            return false;
        }
    }
    else if(ProcedureCallAssist()){}
    else if(IsTokenType(T_MINUS)){
        if (Name()){}
        else if(Number()){}
        else
            return false;
    }
    else if (Number()){}
    else if(IsTokenType(T_STRING_CONST)){}
    else if(IsTokenType(T_TRUE) || IsTokenType(T_FALSE)){}
    else
        return false;
    return true;
}

bool Parser::Name(){
    //std::cout << "Name" << //std::endl;
    if(!Identifier())
        return false;
    
    if (IsTokenType(T_LBRACKET)){
        if(!Expression())
            return false;
        
        if(!IsTokenType(T_RBRACKET)){
            errTable.ReportError(ERROR_MISSING_BRACKET);
            return false;
        }
    }
    return true;
}

bool Parser::ArgumentList(){
    //std::cout << "ArgumentList" << //std::endl;
    if(!Expression())
        return false;
    if(IsTokenType(T_COMMA))
        return ArgumentList();
    return true;
}

bool Parser::Number(){
    //std::cout << "Number" << //std::endl;
    return (IsTokenType(T_INTEGER_CONST) || IsTokenType(T_FLOAT_CONST));
}

bool Parser::String(){
    //std::cout << "String" << //std::endl;
    return IsTokenType(T_STRING_CONST);
}

bool Parser::DeclarationAssist(){
    while(Declaration()){
        if(!IsTokenType(T_SEMICOLON)){
            errTable.ReportError(ERROR_MISSING_SEMICOLON);
            return false;
        }
    }
    return true;
}

bool Parser::StatementAssist(){
    while(Statement()){
        if(!IsTokenType(T_SEMICOLON)){
            errTable.ReportError(ERROR_MISSING_SEMICOLON);
            return false;
        }
    }
    return true;
}


bool Parser::ProcedureCallAssist(){
    //std::cout << "ProcedureCallAssist" << //std::endl;
    if (!IsTokenType(T_IDENTIFIER))
        return false;
    
    if(IsTokenType(T_LPAREN)){
        ArgumentList();

        if (!IsTokenType(T_RPAREN)){
            errTable.ReportError(ERROR_MISSING_PAREN);
            return false;
        }
        return true;  
    }
    else{
        if (IsTokenType(T_LBRACKET)){
            if(!Expression())
                return false;
            
            if(!IsTokenType(T_RBRACKET)){
                errTable.ReportError(ERROR_MISSING_BRACKET);
                return false;
            }
        }
        return true;
    }
}