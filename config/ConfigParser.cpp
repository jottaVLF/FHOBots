#include "ConfigParser.hpp"

ConfigParser::ConfigParser(){
    this->inputConfig.open("config/appConfig.json");
    if(!this->inputConfig.is_open())
        std::cout << "Error on openning appConfig.json!" <<std::endl;

    this->lookAhead = this->getToken();
}

ConfigParser::~ConfigParser(){
    this->inputConfig.close();
}

Config ConfigParser::createConfiguration(){
    std::string field;
    Config config;
    match(BEGIN);
    while(this->lookAhead->type != END){
        if(this->lookAhead->type == PROPERTY){
            field = this->lookAhead->lexeme;
            match(PROPERTY);
            match(SEPARATOR);
            if(field == "camera"){      
                config.camera = stoi(this->lookAhead->lexeme);
                match(VALUE);            
            }else if(field == "communication"){
                config.communication = this->lookAhead->lexeme;
                match(PROPERTY);
            }else if(field == "team-color"){
                config.teamColor = this->lookAhead->lexeme;
                match(PROPERTY);
            }else if(field == "r0"){
                match(BEGIN);
                config.r0 = getRobotConfig();
            }else if(field == "r1"){
                match(BEGIN);
                config.r1 = getRobotConfig();
            }else if(field == "r2"){
                match(BEGIN);
                config.r2 = getRobotConfig();
            }
        }
        if(this->lookAhead->type == PROPERTY_SEPARATOR)
            match(PROPERTY_SEPARATOR);
    }
    match(END);


    return config;
}

RobotConfig ConfigParser::getRobotConfig(){
    RobotConfig config;
    std::string field;
    while(this->lookAhead->type != END){
        if(this->lookAhead->type == PROPERTY){
            field = this->lookAhead->lexeme;
            match(PROPERTY);
            match(SEPARATOR);    
            if(field == "role"){
                config.role = this->lookAhead->lexeme;
                match(PROPERTY);
            }else if(field == "active"){
                config.active = this->lookAhead->lexeme == "true";
                match(PROPERTY);
            }else if(field == "color"){
                config.color = this->lookAhead->lexeme;
                match(PROPERTY);
            }else if(field == "pwm"){
                match(BEGIN);
                config.pwm = getPwmConfig();
            }else if(field == "control"){
                match(BEGIN);
                while(this->lookAhead->type !=END){
                    if(this->lookAhead->type == PROPERTY){
                        if(this->lookAhead->lexeme == "front-left"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.control.frontLeft = getControlConstants();
                        }
                        if(this->lookAhead->lexeme == "front-right"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.control.frontRight = getControlConstants();
                        }if(this->lookAhead->lexeme == "back-left"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.control.backLeft = getControlConstants();
                        }
                        if(this->lookAhead->lexeme == "back-right"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.control.backRight = getControlConstants();
                        }
                    }else if(this->lookAhead->type == PROPERTY_SEPARATOR)
                        match(PROPERTY_SEPARATOR);
                }
                match(END);
            }else if(field == "hardware"){
                match(BEGIN);
                while(this->lookAhead->type !=END){
                    if(this->lookAhead->lexeme == "id"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.hardware.id = stoi(this->lookAhead->lexeme);
                            match(VALUE);
                    }else if(this->lookAhead->lexeme == "xbee"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.hardware.xbee = this->lookAhead->lexeme;
                            match(PROPERTY);
                    }else if(this->lookAhead->lexeme == "chassis"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.hardware.chassis = this->lookAhead->lexeme;
                            match(PROPERTY);
                    }else if(this->lookAhead->lexeme == "pinMotorEsqA"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.hardware.pinMotorEsqA = stoi(this->lookAhead->lexeme);
                            match(VALUE);
                    }else if(this->lookAhead->lexeme == "pinMotorEsqB"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.hardware.pinMotorEsqB = stoi(this->lookAhead->lexeme);
                            match(VALUE);
                    }else if(this->lookAhead->lexeme == "pinMotorDirA"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.hardware.pinMotorDirA = stoi(this->lookAhead->lexeme);
                            match(VALUE);
                    }else if(this->lookAhead->lexeme == "pinMotorDirB"){
                            match(PROPERTY);
                            match(SEPARATOR);
                            config.hardware.pinMotorDirB = stoi(this->lookAhead->lexeme);
                            match(VALUE);
                    }else if(this->lookAhead->type == PROPERTY_SEPARATOR)
                        match(PROPERTY_SEPARATOR);
                }
                match(END);
            }
        }else if(this->lookAhead->type == PROPERTY_SEPARATOR)
            match(PROPERTY_SEPARATOR);
    }
    match(END);
    return config;
}

ControlConstants ConfigParser::getControlConstants(){
    ControlConstants cte;
    std::string field;
    match(BEGIN);
    while(this->lookAhead->type != END){
        if(this->lookAhead->type == PROPERTY){
            field = this->lookAhead->lexeme;
            match(PROPERTY);
            match(SEPARATOR);
            if(field == "kp")
                cte.kp = stof(this->lookAhead->lexeme);
            else if(field == "kd")
                cte.kd = stof(this->lookAhead->lexeme);
            match(VALUE);
        }else if(this->lookAhead->type == PROPERTY_SEPARATOR)
            match(PROPERTY_SEPARATOR);
    }
    match(END);
    return cte;
}

PwmConfig ConfigParser::getPwmConfig(){
    PwmConfig config;
    std::string field;
    while(this->lookAhead->type != END){
        if(this->lookAhead->type == PROPERTY){
            field = this->lookAhead->lexeme;
            match(PROPERTY);
            match(SEPARATOR);
            if(field == "max")
                config.max = stoi(this->lookAhead->lexeme);
            else if(field == "min")
                config.min = stoi(this->lookAhead->lexeme);
            else if(field == "base")
                config.base = stoi(this->lookAhead->lexeme);
            match(VALUE);
        }else if(this->lookAhead->type == PROPERTY_SEPARATOR)
            match(PROPERTY_SEPARATOR);
    }
    match(END);
    return config;
}

void ConfigParser::match(TOKEN_TYPE expected){

     if(this->lookAhead->type == expected){
        this->lookAhead = this->getToken();
        return;
    }
    std::cout << this->lookAhead->type << "\t" << expected << std::endl;
    std::cout << "Error on reading config file!" <<std::endl;
    exit(0);
}

Token * ConfigParser::getToken(){
    std::string buffer = "";

    this->inputConfig >> buffer;

    TOKEN_TYPE type;

    if(buffer.length() > 1 && (buffer[buffer.length()-1] == ',' || buffer[buffer.length()-1] == ':')){
        buffer = buffer.substr(0, buffer.length()-1);
        this->inputConfig.seekg(-1, std::ios_base::cur);
    }
    
    if(buffer == "{")
        type = BEGIN;
    else if(buffer == "}")
        type = END;
    else if(buffer == ":")
        type = SEPARATOR;
    else if(buffer == ",")
        type = PROPERTY_SEPARATOR;
    else if(buffer[0] == '\"'){
        buffer = buffer.substr(1, buffer.length()-2);
        type = PROPERTY;
    }
    else
        type = VALUE;

    return new Token(buffer, type);
}