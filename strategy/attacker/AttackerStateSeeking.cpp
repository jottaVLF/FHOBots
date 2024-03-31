#include "AttackerStateSeeking.hpp"

AttackerStateSeeking::AttackerStateSeeking(Robot* robot) : State("seeking"), _robot(robot)
{}

AttackerStateSeeking::~AttackerStateSeeking()
{}

void AttackerStateSeeking::doActions()
{
    Vector2D destination = Global::ball;

    _robot->calculatePwm(destination);
    _robot->setPwmRight(Global::pwmRightAtt(_robot->getPwmRight()));
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string AttackerStateSeeking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(WorldModel::isAlignedWithWall(_robot->getPosition(), _robot->getOrientation()))
        return "backoff";

    if(WorldModel::isInsideDeffenseArea(_robot->getPosition()))
        return "backoff";
 
    return ""; 
}

void AttackerStateSeeking::entryActions()
{
    _robot->setPD(40, -10);
    _robot->setBasePwmValue(80);
    _robot->setMaxPwm(120);
    _robot->lastPos = _robot->getPosition();
    _robot->lastOri = _robot->getOrientation();
}

void AttackerStateSeeking::exitActions()
{}