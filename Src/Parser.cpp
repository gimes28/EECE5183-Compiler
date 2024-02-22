#include "Parser.h"
#include "Lexer.h"

#include <iostream>

Parser::Parser(Lexer* lexerptr){
    lexer = lexerptr;
};

Parser::~Parser(){

};

bool Parser::IsTokenType(TokenType token){
    if(token == tok.tt){
        tok = lexer->InitScan();
        return true;
    }
    return false;
}

bool Parser::Parse(){
    tok = lexer->InitScan();

    if(!Program()){
        return false;
    }

    while(!IsTokenType(T_EOF)){
        if(!Program()){
            return false;
        }
        std::cout << "Found Program"; 
    }
    return true;
};

bool Parser::Program(){
    if (!ProgramHeader())
        return false;

    if (!ProgramBody())
        return false;

    if (!IsTokenType(T_PERIOD)){
        std::cout << "Error: missing \'.\' at end of program" << std::endl;
        return false;
    }
    return true;
};

bool Parser::ProgramHeader(){
    if (!IsTokenType(T_PROGRAM))
        return false;

    if (!Identifier())
        return false;

    if(!IsTokenType(T_IS)){
        std::cout << "Error: Missing \'is\' in program header" << std::endl;
        return false;
    }
    return true;    
}

bool Parser::ProgramBody(){
    if (!Decleration())
        return false;

    if (IsTokenType(T_BEGIN)){
        std::cout << "Error: Missing \'begin\' in program body" << std::endl;
        return false;
    }

    if (!Statement())
        return false;
    
    if (IsTokenType(T_END)){
        std::cout << "Error: Missing \'end\' in program body" << std::endl;
        return false;
    }

    if (IsTokenType(T_PROGRAM)){
        std::cout << "Error: Missing \'program\' in program body" << std::endl;
        return false;
    }
    return true;
};

bool Parser::Decleration(){
    if (IsTokenType(T_GLOBAL)){
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
    if(IsTokenType(T_PROCEDURE)){
        std::cout << "Error: Missing \'procedure\' in procedure header" << std::endl;
        return false;
    }

    if (!Identifier())
        return false;
    
    if(IsTokenType(T_COLON)){
        std::cout << "Error: Missing \':\' in procedure header" << std::endl;
        return false;
    }

    if(!TypeMark())
        return false;
    
    if(IsTokenType(T_LPAREN)){
        std::cout << "Error: Missing \'(\' in procedure header" << std::endl;
        return false;
    }

    ParameterList();

    if(IsTokenType(T_RPAREN)){
        std::cout << "Error: Missing \')\' in procedure header" << std::endl;
        return false;
    }
    return true;
};

bool Parser::ParameterList(){
    if(!Parameter())
        return false;

    if(IsTokenType(T_COMMA)){
        ParameterList();
    }
    return true;
};

bool Parser::Parameter(){
    if (!VariableDeclaration()){
        return false;
    }   
    return true;
};

bool Parser::ProdecureBody(){
    if (!Decleration())
        return false;

    if (IsTokenType(T_BEGIN)){
        std::cout << "Error: Missing \'begin\' in procedure body" << std::endl;
        return false;
    }

    if (!Statement())
        return false;
    
    if (IsTokenType(T_END)){
        std::cout << "Error: Missing \'end\' in procedure body" << std::endl;
        return false;
    }

    if (IsTokenType(T_PROCEDURE)){
        std::cout << "Error: Missing \'procedure\' in procedure body" << std::endl;
        return false;
    }
    return true;
};

bool Parser::VariableDeclaration(){
    if (!IsTokenType(T_VARIABLE)){
        std::cout << "Error: Missing \'variable\' in variable decleration" << std::endl;
        return false;
    }

    if (!Identifier())
        return false;
    
    if(!IsTokenType(T_COLON)){
        std::cout << "Error: Missing \':\' in variable decleration" << std::endl;
        return false;
    }

    if(!TypeMark())
        return false;
    
    if(IsTokenType(T_LBRACKET)){
        if(!Bound())
            return false;

        if(IsTokenType(T_RBRACKET)){
            std::cout << "Error: Missing \']\' in variable decleration" << std::endl;
            return false;
        }
    }
    return true;
};

bool Parser::TypeMark(){
    if(IsTokenType(T_INTEGER) || IsTokenType(T_DOUBLE) || 
    IsTokenType(T_STRING) || IsTokenType(T_BOOL)){
        return true;
    }
    return false;
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

    if(!IsTokenType(T_LPAREN))
        return false;
    
    ArgumentList();

    if (!IsTokenType(T_RPAREN)){
        std::cout << "Error: Missing \')\' in Procedure Call" << std::endl;
        return false;
    }
    return true;    
}

bool Parser::AssignmentStatement(){
    if(!Destination())
        return false;

    if (!IsTokenType(T_ASSIGNMENT)){
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
    
    if(IsTokenType(T_LBRACKET)){         
        if (!Expression())
            return false;
        
        if (!IsTokenType(T_RPAREN)){
            std::cout << "Error: Missing \')\' in Destination" << std::endl;
            return false;
        }   
    }
    return true; 
}

bool Parser::IfStatement(){
    if (IsTokenType(T_IF)){
        std::cout << "Error: Missing \'if\' in If Statement" << std::endl;
        return false;
    }

    if(IsTokenType(T_LPAREN)){
        std::cout << "Error: Missing \'(\' in If Statement" << std::endl;
        return false;
    }

    if(!Expression())
        return false;

    if(IsTokenType(T_RPAREN)){
        std::cout << "Error: Missing \')\' in If Statement" << std::endl;
        return false;
    }

    if(IsTokenType(T_THEN)){
        std::cout << "Error: Missing \'then\' in If Statement" << std::endl;
        return false;
    }

    if(!Statement())
        return false;

    if(IsTokenType(T_ELSE)){
        if(!Statement())
            return false;
    }

    if(!IsTokenType(T_END)){
        std::cout << "Error: Missing \'End\' in If Statement" << std::endl;
        return false;
    }

    if(!IsTokenType(T_IF)){
        std::cout << "Error: Missing \'If\' in If Statement" << std::endl;
        return false;
    }

    return true; 
}

bool Parser::LoopStatement(){
    if(!IsTokenType(T_FOR)){
        return false;
    }
     
    if (!IsTokenType(T_LPAREN)){
        std::cout << "Error: Missing \'(\' in Loop Statement" << std::endl;
        return false;   
    }

    if(!AssignmentStatement())
        return false;

    if(!IsTokenType(T_SEMICOLON)){
        std::cout << "Error: Missing \';\' in Loop Statement" << std::endl;
        return false;
    }

    if(!Expression())
        return false;

    if(!IsTokenType(T_RPAREN)){
        std::cout << "Error: Missing \')\' in Loop Statement" << std::endl;
        return false;
    }

    if(!Statement())
        return false;
    
    if(!IsTokenType(T_END)){
        std::cout << "Error: Missing \'End\' in Loop Statement" << std::endl;
        return false;
    }

    if(!IsTokenType(T_FOR)){
        std::cout << "Error: Missing \'For\' in Loop Statement" << std::endl;
        return false;
    }
    return true;
}

bool Parser::ReturnStatement(){
    if (IsTokenType(T_RETURN)){
        std::cout << "Error: Missing \'Return\' in Return Statement" << std::endl;
        return false;
    }

    if(!Expression())
        return false;

    return true;
}

bool Parser::Identifier(){
    return (IsTokenType(T_IDENTIFIER));
}

bool Parser::Expression(){
    if(IsTokenType(T_NOT)){

    }
    if(!ArithOp())
        return false;

    if(!ExpressionPrime())
        return false;

    return true;
}

bool Parser::ExpressionPrime(){
    if (IsTokenType(T_AND) || IsTokenType(T_OR)){
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
    if (IsTokenType(T_PLUS) || IsTokenType(T_MINUS)){
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
    if(!Factor())
        return false;
    
    if(!TermPrime())
        return false;

    return true;
}

bool Parser::TermPrime(){
    if(IsTokenType(T_MULTIPLY) || IsTokenType(T_DIVIDE)){
        if(!Factor())
            return false;

        if(!TermPrime())
            return false;
    }
    return true;
}

bool Parser::Factor(){
    if(IsTokenType(T_LPAREN)){
        if(!Expression())
            return false;
        
        if(IsTokenType(T_RPAREN)){
            std::cout << "Error: Missing \')\' in Expression Factor" << std::endl;
            return false;
        }
    }
    else if(ProcedureCall()){
        Name();
    }
    else if(IsTokenType(T_MINUS)){
        Number();
    }
    else if(IsTokenType(T_STRING_CONST)){

    }
    else if(IsTokenType(T_TRUE) || IsTokenType(T_FALSE)){

    }
    else
        return false;
    return true;
}

bool Parser::Name(){
    if(!Identifier())
        return false;
    
    if (IsTokenType(T_LBRACKET)){
        if(!Expression())
            return false;
        
        if(IsTokenType(T_RBRACKET)){
            std::cout << "Error: Missing \']\' in Name" << std::endl;
            return false;
        }
    }
    return true;
}

bool Parser::ArgumentList(){
    if(!Expression())
        return false;
    if(IsTokenType(T_COMMA))
        return ArgumentList();
    return true;
}

bool Parser::Number(){
    return (IsTokenType(T_INTEGER_CONST) || IsTokenType(T_DOUBLE_CONST));
}

bool Parser::String(){
    return IsTokenType(T_STRING_CONST);
}