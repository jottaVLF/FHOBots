#include "DefenderStateWaiting.hpp"


DefenderStateWaiting::DefenderStateWaiting(Robot* robot) : State("waiting"), _robot(robot)
{}

DefenderStateWaiting::~DefenderStateWaiting()
{}

void DefenderStateWaiting::doActions()
{  
    Vector2D destination = WorldModel::getDeffenderDeffencePosition();
    _robot->setPwmLeft(0);
    _robot->setPwmRight(0);
    _robot->setObjective(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string DefenderStateWaiting::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";
    
    if(!WorldModel::isNearOf(_robot->getPosition(), _robot->getObjective()))
        return "seeking";
    
    if(WorldModel::nearstRobotTo(Global::ball) == _robot && !WorldModel::isNearOf(_robot->getPosition(),_robot->getObjective())){
        return "seeking";
    }

    return "";
}

void DefenderStateWaiting::entryActions()
{
    _robot->setPD(105, 45); ///180.5, -950.35
    _robot->setBasePwmValue(45);
    _robot->setMaxPwm(75);
    contador = 0;
}

void DefenderStateWaiting::exitActions()
{}
