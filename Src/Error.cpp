#include "Error.h"

#include <iostream>
#include <iomanip>

Error::Error(){
    errorTable[ERROR_INVALID_INPUT] = "Invalid input";
    errorTable[ERROR_FAIL_TO_OPEN] = "Failed to open";
    errorTable[ERROR_EOF] = "End of file";
    errorTable[ERROR_INVALID_HEADER] = "Invalid header";
    errorTable[ERROR_INVALID_BODY] = "Invalid body";
    errorTable[ERROR_INVALID_DECLARATION] = "Invalid declaration";
    errorTable[ERROR_INVALID_VARIABLE_DECLARATION] = "Invalid variable declaration";
    errorTable[ERROR_INVALID_IDENTIFIER] = "Invalid identifier";
    errorTable[ERROR_INVALID_PARAMETER] = "Invalid parameter";
    errorTable[ERROR_INVALID_BOUND] = "Invalid bound";
    errorTable[ERROR_INVALID_CHARACTER] = "Invalid character";
    errorTable[ERROR_INVALID_FACTOR] = "Invalid factor";
    errorTable[ERROR_INVALID_ARGUMENT] = "Invalid argument";
    errorTable[ERROR_DUPLICATE_IDENTIFIER] = "Identifier exists in scope";
    errorTable[ERROR_DUPLICATE_PROCEDURE] = "Procedure exists in scope";
    errorTable[ERROR_DUPLICATE_VARIABLE] = "Variable exists in scope";    
    errorTable[ERROR_SCOPE_DECLERATION] = "Variable not declared in scope";
    errorTable[ERROR_INVALID_TYPE_CHECK] = "Invalid type check";
    errorTable[ERROR_INVALID_RELATION] = "Incompatible relation operation";
    errorTable[ERROR_INVALID_EXPRESSION] = "Invalid expression operations";
    errorTable[ERROR_INVALID_ASSIGNMENT] = "Invalid assignment";
    errorTable[ERROR_INVALID_RETURN] = "Invalid return";
    
    errorTable[ERROR_INVALID_IF] = "Invalid if statement";
    errorTable[ERROR_INVALID_LOOP] = "Invalid loop";
    errorTable[ERROR_INVALID_RETURN] = "Invalid return";


    errorTable[ERROR_MISSING_SEMICOLON] = "Missing semicolon";
    errorTable[ERROR_MISSING_COLON] = "Missing colon";
    errorTable[ERROR_MISSING_BRACKET] = "Missing bracket";
    errorTable[ERROR_MISSING_PAREN] = "Missing parenthesis";
    errorTable[ERROR_MISSING_PERIOD] = "Missing period";
    errorTable[ERROR_MISSING_STRING_CLOSING] = "Missing string closing quote";
    errorTable[ERROR_MISSING_ASSIGNMENT] = "Missing assignment";
    errorTable[ERROR_MISSING_IDENTIFIER] = "Missing identifier";
}

// Update Error file to include filename and line number

void Error::ReportError(ErrorType err, std::string fileName, int lineNum){
    std::cout << fileName << ":" << std::left << std::setw(15);
    std::cout << lineNum << "Error: " << errorTable[err] << std::endl;
}

void Error::ReportError(ErrorType err, std::string fileName, int lineNum, std::string message){
    std::cout << fileName << ":" << std::left << std::setw(15);
    std::cout << lineNum << "Error: " << errorTable[err] << " -> " << message << std::endl;
}

void Error::ReportError(ErrorType err){
    std::cout << std::left << std::setw(15);
    std::cout << "   Error: " << errorTable[err] << std::endl;
}

void Error::ReportWarning(ErrorType war, std::string fileName, int lineNum){
    std::cout << fileName << ":" << std::left << std::setw(15);
    std::cout << lineNum << "   Warning: " << errorTable[war] << std::endl;
}