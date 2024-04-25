#include "Control.hpp"
#include <cmath>
#include <iostream>

Control::Control(const double kp, const double kd, const double basePwmValue) : _kp(kp), _kd(kd),
_basePwmValue(basePwmValue), _lastError(0), _pwmLeftWheel(0), _pwmRightWheel(0), _maxPwmValue(160)
{}

Control::~Control()
{}

void Control::setPD(const double kp, const double kd)
{
    _kp = kp;
    _kd = kd;
}

void Control::setBasePwmValue(const double pwm)
{
    _basePwmValue = pwm;
}

double Control::getBasePwmValue()
{
    return _basePwmValue;
}

void Control::calculatePwm(Vector2D& robotToDestiny, Vector2D OrientationRobot)
{
    double pd = calculatePD(robotToDestiny, OrientationRobot);

    _pwmLeftWheel = static_cast<int>(_basePwmValue + pd);
    if(_pwmLeftWheel > _maxPwmValue)
        _pwmLeftWheel = _maxPwmValue;
    else if(_pwmLeftWheel < 10)
        _pwmLeftWheel = 10;

    _pwmRightWheel = static_cast<int>(_basePwmValue - pd);
    if(_pwmRightWheel > _maxPwmValue)
        _pwmRightWheel = _maxPwmValue;
    else if(_pwmRightWheel < 10)
        _pwmRightWheel = 10;
}

void Control::calculatePwm(Vector2D& robotToDestiny, const double& angleRobot)
{
    double pd = calculatePD(robotToDestiny, angleRobot);

    _pwmLeftWheel = static_cast<int>(_basePwmValue + pd);
    if(_pwmLeftWheel > _maxPwmValue)
        _pwmLeftWheel = _maxPwmValue;
    else if(_pwmLeftWheel < 10)
        _pwmLeftWheel = 10;

    _pwmRightWheel = static_cast<int>(_basePwmValue - pd);
    if(_pwmRightWheel > _maxPwmValue)
        _pwmRightWheel = _maxPwmValue;
    else if(_pwmRightWheel < 10)
        _pwmRightWheel = 10;
}

void Control::setMaxPwm(const int maxPwm)
{
    _maxPwmValue =  maxPwm;
}

void Control::setPwmLeftWheel(const int pwm)
{
    _pwmLeftWheel = pwm;
}

void Control::setPwmRightWheel(const int pwm)
{
    _pwmRightWheel = pwm;
}

int Control::getPwmLeftWheel()
{
    return _pwmLeftWheel;
}

int Control::getPwmRightWheel()
{
    return _pwmRightWheel;
}

double Control::calculatePD(Vector2D& robotToDestiny, Vector2D& OrientationRobot)
{
    double angleRobot = atan2(OrientationRobot.y, OrientationRobot.x);
    double angleError = calculateError(robotToDestiny, angleRobot);
    double p = angleError;
    double d = angleError - _lastError;
    _lastError = angleError;

    return (_kp * p) + (_kd * d);
}

double Control::calculatePD(Vector2D& robotToDestiny, const double& angleRobot)
{
    Vector2D OrientationRobot(cos(angleRobot), sin(angleRobot));
    double angleError = calculateError(robotToDestiny, angleRobot);

    double p = angleError;
    double d = angleError - _lastError;
    _lastError = angleError;

    return (_kp * p) + (_kd * d);
}

void Control::setLastError(const double lerror)
{
    _lastError = lerror;
}

double Control::calculateError(Vector2D& robotToDestiny, double angleRobot){

    double angleRobotDestiny = atan2(robotToDestiny.y, robotToDestiny.x);
    double error = angleRobotDestiny - angleRobot;

    if( fabs(error) > M_PI){
        if(angleRobotDestiny < 0)
            angleRobotDestiny = 2*M_PI + angleRobotDestiny;

        if(angleRobot < 0)
            angleRobot = 2*M_PI + angleRobot;

        error = angleRobotDestiny - angleRobot;
    }
    return error;

}