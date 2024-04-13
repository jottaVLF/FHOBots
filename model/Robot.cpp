#include "Robot.hpp"
#include "../Global.hpp"
#include <mutex>
#include <chrono>
#include <thread>
#include <sstream>
#include <iostream>

Robot::Robot(const double kp, const double kd, const double basePwmValue) : _control(kp, kd, basePwmValue)
{
    forceSeeking = false;
    this->setObjective(0, 0);
}

Robot::~Robot()
{}

void Robot::calculatePwm(Vector2D &destination)
{
    setObjective(destination);
    Vector2D v = destination - _position;
    _control.calculatePwm(v, _Orientation);
}

void Robot::calculatePwmR(Vector2D &destination)
{
    setObjective(destination);
    Vector2D v = destination - _position;
    _control.calculatePwm(v, (_Orientation * -1));
}

void Robot::setOrientationRobot(const double x, const double y)
{
    _Orientation.set(x, y);
}

void Robot::setOrientationRobot(const Vector2D v)
{
    _Orientation = v;
}

void Robot::setPosition(const double x, const double y)
{
    _position.set(x, y);
}

void Robot::setLastPosition(const double x, const double y)
{
    _lastPosition.set(x, y);
}

void Robot::setPosition(const Vector2D v)
{
    _position = v;
}

Vector2D& Robot::getPosition()
{
    return _position;
}

Vector2D& Robot::getLastPosition()
{
    return _lastPosition;
}

Vector2D& Robot::getOrientation()
{
    return _Orientation;
}

void Robot::setBasePwmValue(const double pwm)
{
    _control.setBasePwmValue(pwm);
}

void Robot::setPwmLeft(const int pwm)
{
    _control.setPwmLeftWheel(pwm);
}

void Robot::setPwmRight(const int pwm)
{
    _control.setPwmRightWheel(pwm);
}

int Robot::getBasePwmValue()
{
    return _control.getBasePwmValue();
}

int Robot::getPwmLeft()
{
    return _control.getPwmLeftWheel();
}

int Robot::getPwmRight()
{
    return _control.getPwmRightWheel();
}

int Robot::getPosMessage()
{
    return _posMessage;
}

std::string Robot::getMessage()
{
    return _machineState.currentState();
}

void Robot::updateRobot()
{
    do
    {
        _machineState.think();
        std::this_thread::sleep_for(std::chrono::microseconds(1000));

    } while(Global::bufferKeyboard != 27);
}

void Robot::setPD(const double kp, const double kd)
{
    _control.setPD(kp, kd);
}

void Robot::setMaxPwm(const int maxPwm)
{
    _control.setMaxPwm(maxPwm);
}

void Robot::setLastError(const double lError)
{
    _control.setLastError(lError);
}

void Robot::setObjective(Vector2D v){
    this->objPos.x = v.x;
    this->objPos.y = v.y;
}

void Robot::setObjective(double x, double y){
    this->objPos.x = x;
    this->objPos.y = y;
}

Vector2D& Robot::getObjective(){
    return this->objPos;
}