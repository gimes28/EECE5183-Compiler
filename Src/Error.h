#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <unordered_map>

enum ErrorType{
    ERROR_NONE = 0,
    ERROR_INVALID_INPUT = 1,
    ERROR_FAIL_TO_OPEN = 2,
    ERROR_EOF = 3,
    ERROR_UNKNOWN = 4,
    ERROR_NO_PROGRAM_END = 100,
    ERROR_NO_OCCURRENCE = 101,
    ERROR_INVALID_HEADER = 102,
    ERROR_INVALID_BODY = 103,
    ERROR_INVALID_DECLARATION = 104,
    ERROR_INVALID_TYPE_MARK = 105,
    ERROR_INVALID_PARAMETER_LIST = 106,
    ERROR_INVALID_BOUND = 107,
    ERROR_INVALID_TYPE_DECLARATION = 108,
    ERROR_INVALID_VARIABLE_DECLARATION = 109,
    ERROR_INVALID_IF = 110,
    ERROR_INVALID_LOOP = 111,
    ERROR_INVALID_RETURN = 112,
    ERROR_INVALID_IDENTIFIER = 113,
    ERROR_INVALID_PARAMETER = 114,
    ERROR_INVALID_CHARACTER = 115,
    ERROR_INVALID_FACTOR = 116,
    ERROR_INVALID_ARGUMENT = 117,
    ERROR_DUPLICATE_IDENTIFIER = 118,
    ERROR_DUPLICATE_PROCEDURE = 119,
    ERROR_DUPLICATE_VARIABLE = 120,
    ERROR_SCOPE_DECLERATION = 121,
    ERROR_MISSING_STRING_CLOSING = 1000,
    ERROR_MISSING_IDENTIFIER = 1001,
    ERROR_MISSING_SEMICOLON = 1002,
    ERROR_MISSING_COLON = 1002,
    ERROR_MISSING_BRACKET = 1004,
    ERROR_MISSING_PAREN = 1005,
    ERROR_MISSING_PERIOD = 1006,
    ERROR_MISSING_ASSIGNMENT = 1007

};

typedef std::unordered_map<ErrorType, std::string> ErrorMap;

class Error{
    public:
        Error(); 
        void ReportError(ErrorType err, std::string fileName, int lineNum);
        void ReportError(ErrorType err, std::string fileName, int lineNum, std::string message);
        void ReportError(ErrorType err);
        
        void ReportWarning(ErrorType err, std::string fileName, int lineNum);  
    private:
        ErrorMap errorTable;
};

extern Error errTable;

#endif