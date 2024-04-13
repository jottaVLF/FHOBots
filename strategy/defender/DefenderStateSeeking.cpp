#include "DefenderStateSeeking.hpp"
#include "../../Global.hpp"
#include <iostream>

DefenderStateSeeking::DefenderStateSeeking(Robot* robot) : State("seeking"), _robot(robot)
{}

DefenderStateSeeking::~DefenderStateSeeking()
{}

void DefenderStateSeeking::doActions()
{
    Vector2D destination(_robot->getPosition().x,Global::ball.y);
    if(WorldModel::isOnDeffenseField(Global::ball)){
        _robot->calculatePwm(Global::ball);
    }
    else{
        _robot->calculatePwm(destination);
    }
        
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());

}

std::string DefenderStateSeeking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";
    if(WorldModel::isNearOf(_robot->getPosition(),_robot->getObjective())){
        return "waiting";
    }
    
    if(WorldModel::isAlignedWithWall(_robot->getPosition(), _robot->getOrientation()))
        return "backoff";

    if(WorldModel::isInsideDeffenseArea(_robot->getPosition()) && WorldModel::isFacingArea(_robot->getOrientation(), Global::areaToDeffend))
        return "backoff";
 
    if(WorldModel::isInsideDeffenseArea(Global::ball) && !WorldModel::isInsideDeffenseArea(_robot->getPosition()))
        return "waiting";

    return "";
}

void DefenderStateSeeking::entryActions()
{
    _robot->setPD(40, 0); ///180.5, -950.35
    _robot->setBasePwmValue(80);
    _robot->setMaxPwm(120);
    atkLastX = Global::attacker.getPosition().x;
}

void DefenderStateSeeking::exitActions()
{
    _robot->forceSeeking = false;
}