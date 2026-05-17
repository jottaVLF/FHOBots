#include "ConfigParser.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <cstdlib>

ConfigParser::ConfigParser() : ConfigParser("config/appConfig.json")
{}

ConfigParser::ConfigParser(const std::string pathConfig) : pathConfig(pathConfig), lookAhead(nullptr)
{}

ConfigParser::~ConfigParser(){
    if(this->inputConfig.is_open())
        this->inputConfig.close();
    delete this->lookAhead;
}

Config ConfigParser::createConfiguration(){
    boost::property_tree::ptree tree;
    try{
        boost::property_tree::read_json(this->pathConfig, tree);
    }catch(const boost::property_tree::json_parser_error& error){
        std::cerr << "Error on reading config file '" << this->pathConfig << "': " << error.message() << std::endl;
        exit(1);
    }

    Config config;
    config.camera = tree.get<int>("camera", 0);
    config.communication = tree.get<std::string>("communication", "");
    config.teamColor = tree.get<std::string>("team-color", "yellow");
    config.simMaxSpeed = tree.get<double>("simulation.max-speed", config.simMaxSpeed);
    config.simSmoothing = tree.get<double>("simulation.smoothing", config.simSmoothing);
    config.simDeadband = tree.get<double>("simulation.deadband", config.simDeadband);
    config.r0 = getRobotConfig(tree.get_child("r0"));
    config.r1 = getRobotConfig(tree.get_child("r1"));
    config.r2 = getRobotConfig(tree.get_child("r2"));
    return config;
}

RobotConfig ConfigParser::getRobotConfig(const boost::property_tree::ptree& tree){
    RobotConfig config;
    config.role = tree.get<std::string>("role", "");
    config.active = tree.get<std::string>("active", "false") == "true";
    config.color = tree.get<std::string>("color", "");
    config.pwm = getPwmConfig(tree.get_child("pwm"));
    config.control.frontLeft = getControlConstants(tree.get_child("control.front-left"));
    config.control.frontRight = getControlConstants(tree.get_child("control.front-right"));
    config.control.backLeft = getControlConstants(tree.get_child("control.back-left"));
    config.control.backRight = getControlConstants(tree.get_child("control.back-right"));
    config.hardware = getHardwareConfig(tree.get_child("hardware"));
    return config;
}

PwmConfig ConfigParser::getPwmConfig(const boost::property_tree::ptree& tree){
    PwmConfig config;
    config.max = tree.get<int>("max", 255);
    config.min = tree.get<int>("min", 0);
    config.base = tree.get<int>("base", 0);
    return config;
}

ControlConstants ConfigParser::getControlConstants(const boost::property_tree::ptree& tree){
    ControlConstants constants;
    constants.kp = tree.get<double>("kp", 0);
    constants.kd = tree.get<double>("kd", 0);
    return constants;
}

HardwareConfig ConfigParser::getHardwareConfig(const boost::property_tree::ptree& tree){
    HardwareConfig config;
    config.id = tree.get<int>("id", 0);
    config.xbee = tree.get<std::string>("xbee", "");
    config.chassis = tree.get<std::string>("chassis", "");
    config.pinMotorEsqA = tree.get<int>("pinMotorEsqA", 0);
    config.pinMotorEsqB = tree.get<int>("pinMotorEsqB", 0);
    config.pinMotorDirA = tree.get<int>("pinMotorDirA", 0);
    config.pinMotorDirB = tree.get<int>("pinMotorDirB", 0);
    return config;
}

RobotConfig ConfigParser::getRobotConfig(){
    return RobotConfig();
}

PwmConfig ConfigParser::getPwmConfig(){
    return PwmConfig();
}

ControlConstants ConfigParser::getControlConstants(){
    return ControlConstants();
}

void ConfigParser::match(TOKEN_TYPE expected){
    if(this->lookAhead != nullptr && this->lookAhead->type == expected){
        this->lookAhead = this->getToken();
        return;
    }
    std::cout << "Legacy parser is disabled. Use JSON configs compatible with boost::property_tree." << std::endl;
    exit(1);
}

Token * ConfigParser::getToken(){
    return new Token("", END);
}
