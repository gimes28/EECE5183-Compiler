#include "Error.h"

#include <vector>
#include <tuple>

typedef std::tuple<ErrorType, std::string> ErrorMap;

std::vector<ErrorMap> ErrorList;
std::vector<ErrorMap> WarningList;

void Error::ReportError(ErrorType err, std::string message){
    ErrorList.push_back(ErrorMap(err, message));
}

void Error::ReportWarning(ErrorType err, std::string message){
    WarningList.push_back(ErrorMap(err, message));
}