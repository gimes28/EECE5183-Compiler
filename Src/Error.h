#ifndef ERROR_H
#define ERROR_H

#include <string>

typedef int ErrorType;

#define ERROR_NONE 0
#define ERROR_FAIL_TO_OPEN 1
#define ERROR_EOF 2
#define ERROR_UNKNOWN 3

namespace Error{
    void ReportError(ErrorType err, std::string message);
    void ReportWarning(ErrorType err, std::string message);  
};

#endif