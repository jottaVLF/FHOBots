#ifndef FHOBOTS_CONFIG_PARSER_HPP
#define FHOBOTS_CONFIG_PARSER_HPP

#include <fstream>
#include <iostream>
#include <string>
#include "Config.hpp"
#include "Token.hpp"

class ConfigParser{
public:
    ConfigParser();
    ~ConfigParser();
    Config createConfiguration();

private:
    std::ifstream inputConfig;
    void match(TOKEN_TYPE expected);
    Token * lookAhead;
    Token * getToken();
    RobotConfig getRobotConfig(); 
    PwmConfig getPwmConfig();
    ControlConstants getControlConstants();
};





#endif