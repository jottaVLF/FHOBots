#ifndef FHOBOTS_ILOGGER
#define FHOBOTS_ILOGGER

#include <fstream>
#include <iostream>
#include <ctime>

class Logger{
private:
    std::string filePath;
    std::ofstream file;
public:

    Logger(std::string filePath);
    void logInformation(std::string message);
    void logError(std::string message);
    void logWarning(std::string message);
};




#endif