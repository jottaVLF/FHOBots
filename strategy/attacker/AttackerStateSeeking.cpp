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
    bool isAlignedWithBall =        WorldModel::isAlignedWith(_robot->getOrientation(), Global::ball - _robot->getPosition());
    bool isAlignedWithAttackGoal =  WorldModel::isAlignedWith(_robot->getOrientation(), Global::areaGoalAttack - _robot->getPosition());

    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(isAlignedWithBall && isAlignedWithAttackGoal)
        return "attacking";

    if(WorldModel::isAlignedWithWall(_robot->getPosition(), _robot->getOrientation()))
        return "backoff";

    if(WorldModel::isInsideDeffenseArea(_robot->getPosition()) && WorldModel::isFacingArea(_robot->getOrientation(), Global::areaToDeffend))
        return "backoff";
 
    if(WorldModel::isInsideDeffenseArea(Global::ball) && !WorldModel::isInsideDeffenseArea(_robot->getPosition()))
        return "waiting";

    return ""; 
}

void AttackerStateSeeking::entryActions()
{
    _robot->setPD(40, 0);
    _robot->setBasePwmValue(60);
    _robot->setMaxPwm(120);
    _robot->lastPos = _robot->getPosition();
    _robot->lastOri = _robot->getOrientation();
}

void AttackerStateSeeking::exitActions()
{}