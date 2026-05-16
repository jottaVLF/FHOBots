#include "AttackerStateAlign.hpp"
#include "../../Global.hpp"
#include <iostream>

AttackerStateAlign::AttackerStateAlign(Robot *robot) : State("align"), _robot(robot)
{}

AttackerStateAlign::~AttackerStateAlign()
{}

void AttackerStateAlign::doActions()
{
    Vector2D ballToRobot = Global::ball - _robot->getPosition();
    if(_robot->getErrorAngleTo(ballToRobot) > 0)
        _robot->spinClockWise(160);
    else
        _robot->spinCounterClockWise(160);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight(), _robot->reverseLeft, _robot->reverseRight);
}

std::string AttackerStateAlign::checkConditions()
{
    Vector2D ballToRobot = Global::ball - _robot->getPosition();
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(fabs(_robot->getErrorAngleTo(ballToRobot)) < M_PI / 10)
        return "seeking";

    if(Global::robotNearRobot(_robot))
        return "backoff";

    if(WorldModel::isAlignedWithWall(_robot->getPosition(), _robot->getOrientation()))
        return "backoff";


    return "";
}

void AttackerStateAlign::entryActions()
{
    _robot->setPD(100.55, 425);
    _robot->setMaxPwm(120);
    _robot->setBasePwmValue(100);
}

void AttackerStateAlign::exitActions()
{
    _robot->moveForward(0);
    _robot->setMaxPwm(160);
}