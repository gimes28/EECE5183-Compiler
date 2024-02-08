#ifndef ERROR_H
#define ERROR_H

#include <string>

typedef int ErrorType;

#define ERROR_NONE 0
#define ERROR_INVALID_TYPE 1
#define ERROR_FAIL_TO_OPEN 2
#define ERROR_EOF 3
#define ERROR_UNKNOWN 4

namespace Error{
    void ReportError(ErrorType err, std::string message);
    void ReportWarning(ErrorType err, std::string message);  

    void PrintErrorList();
    void PrintWarningList();
};

#endif