#include "DefenderStateWaiting.hpp"
#include "../../Global.hpp"

DefenderStateWaiting::DefenderStateWaiting(Robot* robot) : State("waiting"), _robot(robot)
{}

DefenderStateWaiting::~DefenderStateWaiting()
{}

void DefenderStateWaiting::doActions()
{
    Vector2D destination(_robot->getPosition().x,Global::ball.y);
    if(WorldModel::isOnDeffenseField(Global::ball)){
        _robot->setObjective(Global::ball);
    }
    else{
        _robot->setObjective(destination);
    }
    
    _robot->setPwmLeft(0);
    _robot->setPwmRight(0);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string DefenderStateWaiting::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";
    if(!WorldModel::isNearOf(_robot->getPosition(),_robot->getObjective())){
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
