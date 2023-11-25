#ifndef FHOBOTS_CONFIG_HPP
#define FHOBOTS_CONFIG_HPP

class PwmConfig{
public:
    int max;
    int min;
    int base;
};

class ControlConstants{
public:
    double kp;
    double kd;
};

class ControlConfig{
public:
    ControlConstants frontLeft;
    ControlConstants frontRight;
    ControlConstants backLeft;
    ControlConstants backRight;
};

class RobotConfig{
public:
     std::string role;
     std::string color;
     PwmConfig pwm;
     ControlConfig control;
};

class Config{
public:
    int camera;
    std::string communication;
    int simulatorPort;
    std::string simulatorAddress;
    std::string teamColor;
    std::string simulatorPath;
    std::string simulatorScene;
    RobotConfig r0; 
    RobotConfig r1;
    RobotConfig r2;
};

#endif