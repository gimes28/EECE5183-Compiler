#ifndef FILE_IN_H
#define FILE_IN_H

#include<string>
#include<fstream>

class FileIn{
    private:
        std::string m_fileInName;
    public:
        FileIn();
        FileIn(std::string fileInName);

        

        void SetFileInName(std::string fileInName);
        std::string GetFileInName();
    
};

#endif