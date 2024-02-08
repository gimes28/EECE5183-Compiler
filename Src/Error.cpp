#include "Error.h"

#include <iostream>
#include <vector>
#include <tuple>

typedef std::tuple<ErrorType, std::string> ErrorTuple;

std::vector<ErrorTuple> ErrorList;
std::vector<ErrorTuple> WarningList;

void Error::ReportError(ErrorType err, std::string message){
    ErrorList.push_back(ErrorTuple(err, message));
}

void Error::ReportWarning(ErrorType war, std::string message){
    WarningList.push_back(ErrorTuple(war, message));
}

void Error::PrintErrorList(){
    for (ErrorTuple err : ErrorList){
        std::cout << "Error: " << std::get<0>(err) << " " << std::get<1>(err) << std::endl;
    }
}

void Error::PrintWarningList(){
    for (ErrorTuple war : WarningList){
        std::cout << "Error: " << std::get<0>(war) << " " << std::get<1>(war) << std::endl;
    }
}