#include "DefenderStateAlign.hpp"
#include "../../Global.hpp"
#include "iostream"

DefenderStateAlign::DefenderStateAlign(Robot *robot) : State("align"), _robot(robot)
{}

DefenderStateAlign::~DefenderStateAlign()
{}

void DefenderStateAlign::doActions()
{
    Vector2D destination = Global::ball;

    Vector2D oriAux = destination - _robot->getPosition();

    if((_robot->getOrientation()||oriAux) >= M_PI/4)
        Global::communication->writeMessage(_robot->getPosMessage(), 2, 38);
    else if((_robot->getOrientation()||oriAux) <= -M_PI/4){
        Global::communication->writeMessage(_robot->getPosMessage(), 3, 38);
    }
    else{
        Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
        align = true;
    }
}

std::string DefenderStateAlign::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(align)
        return "seeking";

    return "";
}

void DefenderStateAlign::entryActions()
{
    if(Global::isSim){
        // Calibrações para o simulador
        _robot->setPD(100, 400.5);
        _robot->setBasePwmValue(70);
        _robot->setMaxPwm(255);
    }else{
        _robot->setPD(100, 400.5);
        _robot->setBasePwmValue(70);
        _robot->setMaxPwm(255);
    }
    align = false;
}

void DefenderStateAlign::exitActions()
{
    _robot->setMaxPwm(160);
    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}