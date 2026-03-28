#include "DefenderStateKicking.hpp"
#include "../../Global.hpp"

DefenderStateKicking::DefenderStateKicking(Robot* robot) : State("kicking"), _robot(robot)
{}

DefenderStateKicking::~DefenderStateKicking()
{}

void DefenderStateKicking::doActions()
{
    Vector2D destination = Global::areaGoalAttack.getCenter();
    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string DefenderStateKicking::checkConditions()
{

    Vector2D robotToGol = Global::areaGoalAttack-_robot->getPosition();
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(!WorldModel::isAlignedWith(_robot->getOrientation(), Global::ball - _robot->getPosition()))
        return "seeking";

    return "";
}

void DefenderStateKicking::entryActions()
{
    if(Global::isSim){
        // Calibrações para o simulador
        _robot->setPD(90, 50);
        _robot->setBasePwmValue(190);
    }else{
        _robot->setPD(90, 50);
        _robot->setBasePwmValue(190);
    }
}

void DefenderStateKicking::exitActions()
{}