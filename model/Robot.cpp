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
    this->reverseLeft = false;
    this->reverseRight= false;
    _velocity.set(0, 0);
}

Robot::~Robot()
{}

double Robot::calculateError(Vector2D& robotToDestiny, double angleRobot ){

    return _control.calculateError(robotToDestiny, angleRobot);
}
void Robot::calculatePwm(Vector2D &destination)
{
    setObjective(destination);
    Vector2D v = destination - _position;
    _control.calculatePwm(v, _Orientation * 50);
}

void Robot::calculatePwmR(Vector2D &destination)
{
    setObjective(destination);
    Vector2D v = destination - _position;
    _control.calculatePwm(v, _Orientation * -50);
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

Vector2D& Robot::getVelocity()
{
    return _velocity;
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

void Robot::setVelocity(const Vector2D v)
{
    _velocity = v;
}

int Robot::getBasePwmValue()
{
    return _control.getBasePwmValue();
}

double Robot::getError()
{
    return _control.getError();
}

double Robot::getPD()
{
    return _control.getPD();
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
    while(Global::running.load())
    {
        {
            std::lock_guard<std::mutex> lock(Global::worldMutex);
            _machineState.think();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void Robot::think()
{
    _machineState.think();
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


void Robot::spinOverLeftWheel(int pwm){
    this->reverseLeft = false;
    this->reverseRight= true;
    _control.setPwmLeftWheel(0);
    _control.setPwmRightWheel(pwm);
}

void Robot::spinOverRightWheel(int pwm){
    this->reverseLeft = true;
    this->reverseRight= false;
    _control.setPwmLeftWheel(pwm);
    _control.setPwmRightWheel(0);
}

void Robot::spinClockWise(int pwm){
    this->reverseLeft = false;
    this->reverseRight= true;
    _control.setPwmLeftWheel(pwm);
    _control.setPwmRightWheel(pwm);
}

void Robot::spinCounterClockWise(int pwm){
    this->reverseLeft  = true;
    this->reverseRight = false;
    _control.setPwmLeftWheel(pwm);
    _control.setPwmRightWheel(pwm);
}


void Robot::moveForward(int pwm){
    this->reverseLeft  = false;
    this->reverseRight = false;
    _control.setPwmLeftWheel(pwm);
    _control.setPwmRightWheel(pwm);
}

void Robot::moveBackward(int pwm){
    this->reverseLeft  = true;
    this->reverseRight = true;
    _control.setPwmLeftWheel(pwm);
    _control.setPwmRightWheel(pwm);
}

double Robot::getErrorAngleTo(Vector2D objective){
    return _control.calculateError(objective, _Orientation.angle());
}
