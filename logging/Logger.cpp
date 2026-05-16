#include "Logger.hpp"

Logger::Logger(std::string filePath){
    this->file.open(filePath);
    if(!this->file.is_open())
        std::cout << "Error on openning the file " << filePath << std::endl;
}

void Logger::logInformation(std::string message){
    time_t now = time(0);
    this->file << ctime(&now) << " - INFO: " << message << std::endl;
}

void Logger::logError(std::string message){
    time_t now = time(0);
    this->file << ctime(&now) << " - ERROR: "<< message << std::endl;
}

void Logger::logWarning(std::string message){
    time_t now = time(0);
    this->file <<  ctime(&now) << " - WARN: " << message << std::endl;
}