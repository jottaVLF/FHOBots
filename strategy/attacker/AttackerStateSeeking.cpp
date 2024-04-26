#include "AttackerStateSeeking.hpp"

AttackerStateSeeking::AttackerStateSeeking(Robot* robot) : State("seeking"), _robot(robot)
{}

AttackerStateSeeking::~AttackerStateSeeking()
{}

void AttackerStateSeeking::doActions()
{   
    Vector2D destination = Global::ball;
    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string AttackerStateSeeking::checkConditions()
{
    Vector2D ballToRobot = Global::ball - _robot->getPosition();   
    bool isAlignedWithBall =        WorldModel::isAlignedWith(_robot->getOrientation(), Global::ball - _robot->getPosition());
    bool isAlignedWithAttackGoal =  WorldModel::isAlignedWith(_robot->getOrientation(), Global::areaGoalAttack - _robot->getPosition());

    if(Global::bufferKeyboard == (int)'p')
        return "idle";
    
    if(fabs(_robot->getErrorAngleTo(ballToRobot)) > M_PI/2)
        return "align";

    if(isAlignedWithBall && isAlignedWithAttackGoal)
        return "attacking";
    
    if(WorldModel::isAlignedWithWallAndBall(_robot->getPosition(), _robot->getOrientation()))
        return "spinning";   

    if(WorldModel::isOnDeffenseField(_robot->getPosition()) &&
       WorldModel::isOnDeffenseField(Global::ball) && 
       WorldModel::nearstRobotTo(Global::ball) != _robot)
        return "waiting";
    
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
    Vector2D dist = Global::ball - _robot->getPosition();
    _robot->setPD(15, 70);
    _robot->setBasePwmValue(40);
    _robot->setMaxPwm(80);
    _robot->lastPos = _robot->getPosition();
    _robot->lastOri = _robot->getOrientation();
}

void AttackerStateSeeking::exitActions()
{}