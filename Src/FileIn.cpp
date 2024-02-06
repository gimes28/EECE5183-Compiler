#include "FileIn.h"

FileIn::FileIn(){
    FileIn("");
}

FileIn::FileIn(std::string fileInName){
    m_fileInName = fileInName;
}

void SetFileInName (std::string fileInName){
    m_fileInName = fileInName;
}

std::string GetFileInName(){
    return m_fileInName;
}



