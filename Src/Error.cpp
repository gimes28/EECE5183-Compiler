#include "Error.h"

#include <iostream>
#include <iomanip>

Error::Error(){
    errorTable[ERROR_INVALID_INPUT] = "Invalid input";
    errorTable[ERROR_FAIL_TO_OPEN] = "Failed to open";
    errorTable[ERROR_EOF] = "End of file";
    errorTable[ERROR_MISSING_STRING_CLOSING] = "String missing ending quote";
}

// Update Error file to include filename and line number

void Error::ReportError(ErrorType err, std::string fileName, int lineNum){
    std::cout << fileName << ":" << std::left << std::setw(15);
    std::cout << lineNum << "   Error: " << errorTable[err] << std::endl;
}

void Error::ReportWarning(ErrorType war, std::string fileName, int lineNum){
    std::cout << fileName << ":" << std::left << std::setw(15);
    std::cout << lineNum << "   Warning: " << errorTable[war] << std::endl;
}