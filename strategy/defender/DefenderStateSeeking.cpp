#include "DefenderStateSeeking.hpp"
#include "DefenderUnivector.hpp"
#include "../../Global.hpp"
#include <iostream>

DefenderStateSeeking::DefenderStateSeeking(Robot* robot) : State("seeking"), _robot(robot)
{}

DefenderStateSeeking::~DefenderStateSeeking()
{}

void DefenderStateSeeking::doActions()
{
    Vector2D destination = (WorldModel::isOnDeffenseField(Global::ball) &&
                            WorldModel::nearstRobotTo(Global::ball) == _robot)
                           ? Global::ball
                           : WorldModel::getDeffenderDeffencePosition();

    if(DefenderUnivector::applyNavigation(_robot, destination))
        return;

    // fallback sem visão ou campo inválido
    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string DefenderStateSeeking::checkConditions()
{
    Vector2D robotToGol = Global::areaGoalAttack-_robot->getPosition();
    if(Global::bufferKeyboard == (int)'p')
        return "idle";
    
    if(WorldModel::isAlignedWith(_robot->getOrientation(),robotToGol)&& WorldModel::isNearOf(Global::ball, _robot->getPosition()))
        return "kicking";
    
    /*if(WorldModel::isAlignedWithWallAndBall(_robot->getPosition(), _robot->getOrientation()))
        return "spinning"; 
    
    if(WorldModel::isOnDeffenseField(Global::ball)&& WorldModel::isNearOf(_robot->getPosition(),Global::ball)){
        return "spinning";
    }
    if(WorldModel::isNearOf(_robot->getPosition(),_robot->getObjective())){
        return "waiting";
    }
    
    if(WorldModel::nearstRobotTo(Global::ball) != _robot && WorldModel::isNearOf(_robot->getPosition(), Global::attacker.getPosition())){
        return "waiting";
    }
    */
    if(WorldModel::isAlignedWithWall(_robot->getPosition(), _robot->getOrientation()))
        return "backoff";

    if(WorldModel::isInsideDeffenseArea(_robot->getPosition()) && WorldModel::isFacingArea(_robot->getOrientation(), Global::areaToDeffend))
        return "backoff";
    

    /*if(WorldModel::isInsideDeffenseArea(Global::ball) && !WorldModel::isInsideDeffenseArea(_robot->getPosition()))
        return "waiting";*/

    return "";
}

void DefenderStateSeeking::entryActions()
{
    _robot->setPD(10, 60);
    _robot->setBasePwmValue(200);
    _robot->setMaxPwm(100);
    atkLastX = Global::attacker.getPosition().x;
}

void DefenderStateSeeking::exitActions()
{
    _robot->forceSeeking = false;
}