#include "StateIdle.hpp"
#include "../../Global.hpp"

StateIdle::StateIdle(Robot* robot) : State("idle"), _robot(robot)
{}

StateIdle::~StateIdle()
{}

void StateIdle::doActions()
{}

std::string StateIdle::checkConditions()
{
    if(Global::bufferKeyboard == 32)
        return "seeking";

    return "";
}

void StateIdle::entryActions()
{
    if(Global::isSim){
        // Calibrações para o simulador
        _robot->setPwmLeft(0);
        _robot->setPwmRight(0);
    }else{
        _robot->setPwmLeft(0);
        _robot->setPwmRight(0);
    }
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
    Global::attacker.setPosition(0, 0);
    Global::deffender.setPosition(0, 0);
    Global::goalkeeper.setPosition(0, 0);
    Global::ballPos.set(-1, -1);
}

void StateIdle::exitActions()
{
    Vector2D robotToDestiny = Global::ball - _robot->getPosition();
    _robot->setLastError((_robot->getOrientation()||robotToDestiny));
}
