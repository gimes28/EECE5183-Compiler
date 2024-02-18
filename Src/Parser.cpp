#include "Parser.h"
#include "Lexer.h"

#include <iostream>

Parser::Parser(Lexer* lexerptr){
    lexer = lexerptr;
};

Parser::~Parser(){

};

bool Parser::Parse(){
    tok = lexer->ScanToken();

    if(!Program()){
        return false;
    }

    while(tok.tt != T_EOF){
        if(!Program()){
            return false;
        }
    }
    return true;
};

bool Parser::Program(){
    if (!ProgramHeader())
        return false;

    if (!ProgramBody())
        return false;

    if (!tok.tt != T_PERIOD){
        std::cout << "Error: missing \'.\' at end of program" << std::endl;
        return false;
    }
    return true;
};

bool Parser::ProgramHeader(){
    if (!tok.tt != T_PROGRAM)
        return false;

    if (!Identifier())
        return false;

    if(!tok.tt != T_IS){
        std::cout << "Error: Missing \'is\' in program header" << std::endl;
        return false;
    }
    return true;    
}

bool Parser::ProgramBody(){
    if (!Decleration())
        return false;

    if (tok.tt != T_BEGIN){
        std::cout << "Error: Missing \'begin\' in program body" << std::endl;
        return false;
    }

    if (!Statement())
        return false;
    
    if (tok.tt != T_END){
        std::cout << "Error: Missing \'end\' in program body" << std::endl;
        return false;
    }

    if (tok.tt != T_PROGRAM){
        std::cout << "Error: Missing \'program\' in program body" << std::endl;
        return false;
    }
    return true;
};

bool Parser::Decleration(){
    if (tok.tt != T_GLOBAL){
        std::cout << "Error: Missing \'global\' in decleration" << std::endl;
        return false;
    }

    if(ProcedureDeclaration()){

    }
    else if (VariableDeclaration()){

    }
    else{
        return false;
    }
    return true;
};

bool Parser::ProcedureDeclaration(){
    if(!ProcedureHeader())
        return false;

    if(!ProdecureBody())
        return false;
    return true;
};

bool Parser::ProcedureHeader(){
    if(tok.tt != T_PROCEDURE){
        std::cout << "Error: Missing \'procedure\' in procedure header" << std::endl;
        return false;
    }

    if (!Identifier())
        return false;
    
    if(tok.tt != T_COLON){
        std::cout << "Error: Missing \':\' in procedure header" << std::endl;
        return false;
    }

    if(!TypeMark())
        return false;
    
    if(tok.tt != T_LPAREN){
        std::cout << "Error: Missing \'(\' in procedure header" << std::endl;
        return false;
    }

    ParameterList();

    if(tok.tt != T_RPAREN){
        std::cout << "Error: Missing \')\' in procedure header" << std::endl;
        return false;
    }
};

bool Parser::ParameterList(){
    if(!Parameter())
        return false;

    if(tok.tt == T_COMMA){
        ParameterList();
    }
    return true;
};

bool Parser::Parameter(){
    if (!VariableDeclaration()){
        return false;
    }   
};

bool Parser::ProdecureBody(){
    if (!Decleration())
        return false;

    if (tok.tt != T_BEGIN){
        std::cout << "Error: Missing \'begin\' in procedure body" << std::endl;
        return false;
    }

    if (!Statement())
        return false;
    
    if (tok.tt != T_END){
        std::cout << "Error: Missing \'end\' in procedure body" << std::endl;
        return false;
    }

    if (tok.tt != T_PROCEDURE){
        std::cout << "Error: Missing \'procedure\' in procedure body" << std::endl;
        return false;
    }
    return true;
};

bool Parser::VariableDeclaration(){
    if (tok.tt != T_VARIABLE){
        std::cout << "Error: Missing \'variable\' in variable decleration" << std::endl;
        return false;
    }

    if (!Identifier())
        return false;
    
    if(tok.tt != T_COLON){
        std::cout << "Error: Missing \':\' in variable decleration" << std::endl;
        return false;
    }

    if(!TypeMark())
        return false;
    
    if(tok.tt != T_LBRACKET){
        std::cout << "Error: Missing \'[\' in variable decleration" << std::endl;
        return false;
    }

    if(!Bound())
        return false;

    if(tok.tt != T_RBRACKET){
        std::cout << "Error: Missing \']\' in variable decleration" << std::endl;
        return false;
    }
};

bool Parser::TypeMark(){
    if(tok.tt == T_INTEGER || tok.tt == T_DOUBLE || 
    tok.tt == T_STRING || tok.tt == T_BOOL){
        return true;
    }
}

bool Parser::Bound(){
    return Number();
}

bool Parser::Statement(){
    if (AssignmentStatement()){

    }
    else if (IfStatement()){

    }
    else if (LoopStatement()){

    }
    else if (ReturnStatement()){

    }
    else 
        return false;
    return true;
}

bool Parser::ProcedureCall(){
    if(!Identifier())
        return false;

    if(!tok.tt != T_LPAREN)
        return false;
    
    ArgumentList();

    if (!tok.tt != T_RPAREN){
        std::cout << "Error: Missing \')\' in Procedure Call" << std::endl;
        return false;
    }
    return true;    
}

bool Parser::AssignmentStatement(){
    if(!Destination())
        return false;

    if (tok.tt != T_ASSIGNMENT){
        std::cout << "Error: Missing \':=\' in Assignment Statement" << std::endl;
        return false;
    }

    if (!Expression())
        return false;
    
    return true;    
}

bool Parser::Destination(){
    if(!Identifier())
        return false;
    
    if(tok.tt != T_LBRACKET){
        
    }

    if (!Expression())
        return false;
    
    if (!tok.tt != T_RPAREN){
        std::cout << "Error: Missing \')\' in Destination" << std::endl;
        return false;
    }
    return true; 
}

bool Parser::IfStatement(){
    if (tok.tt != T_IF){
        std::cout << "Error: Missing \'if\' in If Statement" << std::endl;
        return false;
    }

    if(tok.tt != T_LPAREN){
        std::cout << "Error: Missing \'(\' in If Statement" << std::endl;
        return false;
    }

    if(!Expression())
        return false;

    if(tok.tt != T_RPAREN){
        std::cout << "Error: Missing \')\' in If Statement" << std::endl;
        return false;
    }

    if(tok.tt != T_THEN){
        std::cout << "Error: Missing \'then\' in If Statement" << std::endl;
        return false;
    }

    if(!Statement())
        return false;

    if(tok.tt == T_ELSE){
        if(!Statement())
            return false;
    }

    if(!tok.tt != T_END){
        std::cout << "Error: Missing \'End\' in If Statement" << std::endl;
        return false;
    }

    if(!tok.tt != T_IF){
        std::cout << "Error: Missing \'If\' in If Statement" << std::endl;
        return false;
    }

    return true; 
}

bool Parser::LoopStatement(){
    if(tok.tt != T_FOR){
        return false;
    }
     
    if (tok.tt != T_LPAREN){
        std::cout << "Error: Missing \'(\' in Loop Statement" << std::endl;
        return false;   
    }

    if(!AssignmentStatement())
        return false;

    if(tok.tt != T_SEMICOLON){
        std::cout << "Error: Missing \';\' in Loop Statement" << std::endl;
        return false;
    }

    if(!Expression())
        return false;

    if(tok.tt != T_RPAREN){
        std::cout << "Error: Missing \')\' in Loop Statement" << std::endl;
        return false;
    }

    if(!Statement())
        return false;
    
    if(!tok.tt != T_END){
        std::cout << "Error: Missing \'End\' in Loop Statement" << std::endl;
        return false;
    }

    if(!tok.tt != T_FOR){
        std::cout << "Error: Missing \'For\' in Loop Statement" << std::endl;
        return false;
    }
    return true;
}

bool Parser::ReturnStatement(){
    if (tok.tt != T_RETURN){
        std::cout << "Error: Missing \'Return\' in Return Statement" << std::endl;
        return false;
    }

    if(!Expression())
        return false;

    return true;
}

bool Parser::Identifier(){
    return (tok.tt != T_IDENTIFIER);
}

bool Parser::Expression(){
    if(tok.tt != T_NOT){

    }
    if(!ArithOp())
        return false;

    if(!ExpressionPrime())
        return false;

    return true;
}

bool Parser::ExpressionPrime(){
    if (tok.tt != T_AND || tok.tt != T_OR){
        if(!ArithOp())
            return false;
        
        if(!ExpressionPrime())
            return false;
    }
    return true;
}

bool Parser::ArithOp(){
    if(!Relation())
        return false;
    
    if(!ArithOpPrime())
        return false;
    
    return true;
}

bool Parser::ArithOpPrime(){
    if (tok.tt != T_PLUS || tok.tt != T_MINUS){
        if(!Relation())
            return false;
        
        if(!ArithOpPrime())
            return false;
    }
    return true;
}

bool Parser::Relation(){
    if(!Term())
        return false;
    
    if(!RelationPrime())
        return false;

    return true;
}

bool Parser::RelationPrime(){
    if(tok.tt != T_LESS || tok.tt != T_LESS_EQ || 
        tok.tt != T_GREATER || tok.tt != T_GREATER_EQ || 
        tok.tt != T_EQUAL || tok.tt != T_NOT_EQUAL){
        if(!Term())
            return false;
        if(!RelationPrime)
            return false;
    }
    return true;
}

bool Parser::Term(){
    if(!Factor())
        return false;
    
    if(!TermPrime())
        return false;

    return true;
}

bool Parser::TermPrime(){
    if(tok.tt != T_MULTIPLY || tok.tt != T_DIVIDE){
        if(!Factor())
            return false;

        if(!TermPrime)
            return false;
    }
    return true;
}

bool Parser::Factor(){
    if(tok.tt == T_LPAREN){
        if(!Expression())
            return false;
        
        if(tok.tt != T_RPAREN){
            std::cout << "Error: Missing \')\' in Expression Factor" << std::endl;
            return false;
        }
    }
    else if(ProcedureCall()){
        Name();
    }
    else if(tok.tt == T_MINUS){
        Number();
    }
    else if(tok.tt == T_STRING_CONST){

    }
    else if(tok.tt == T_TRUE || tok.tt == T_FALSE){

    }
    else
        return false;
    return true;
}

bool Parser::Name(){
    if(!Identifier())
        return false;
    
    if (tok.tt == T_LBRACKET){
        if(!Expression())
            return false;
        
        if(tok.tt != T_RBRACKET){
            std::cout << "Error: Missing \']\' in Name" << std::endl;
            return false;
        }
    }
    return true;
}

bool Parser::Number(){
    return (tok.tt == T_INTEGER_CONST || tok.tt == T_DOUBLE_CONST);
}

bool Parser::String(){
    return tok.tt == T_STRING_CONST;
}