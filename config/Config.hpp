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

class HardwareConfig{
public:
    int id;
    int pinMotorEsqA;
    int pinMotorEsqB;
    int pinMotorDirA;
    int pinMotorDirB;
};

class RobotConfig{
public:
     std::string role;
     std::string color;
     bool active;
     PwmConfig pwm;
     ControlConfig control;
     HardwareConfig hardware;
};

class Config{
public:
    int camera;
    std::string communication;
    std::string teamColor;
    RobotConfig r0; 
    RobotConfig r1;
    RobotConfig r2;
};

#endif