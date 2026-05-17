#ifndef FHOBOTS_CONFIG_PARSER_HPP
#define FHOBOTS_CONFIG_PARSER_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "Config.hpp"
#include "Token.hpp"

class ConfigParser{
public:
    ConfigParser();
    ConfigParser(const std::string pathConfig);
    ~ConfigParser();
    Config createConfiguration();

private:
    std::ifstream inputConfig;
    std::string pathConfig;
    void match(TOKEN_TYPE expected);
    Token * lookAhead;
    Token * getToken();
    RobotConfig getRobotConfig(const boost::property_tree::ptree& tree);
    PwmConfig getPwmConfig(const boost::property_tree::ptree& tree);
    ControlConstants getControlConstants(const boost::property_tree::ptree& tree);
    HardwareConfig getHardwareConfig(const boost::property_tree::ptree& tree);
    RobotConfig getRobotConfig(); 
    PwmConfig getPwmConfig();
    ControlConstants getControlConstants();
};





#endif
