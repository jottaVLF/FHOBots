#include "StateBackOff.hpp"
#include "../../Global.hpp"

StateBackOff::StateBackOff(Robot* robot) : State("backoff"), _robot(robot), _side(0)
{}

StateBackOff::~StateBackOff()
{}

void StateBackOff::doActions()
{
    _robot->moveBackward(100);
    Global::communication->writeMessage(_robot->getPosMessage(),80, 80, true,true);
}

std::string StateBackOff::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";
    
    if(!WorldModel::isAlignedWithWall(_robot->getPosition(), _robot->getOrientation()))
        return "seeking";

    return "";
}

void StateBackOff::entryActions()
{    
        _robot->setPwmRight(0);
        _robot->setPwmLeft(0);
    newAngle = _robot->getOrientation().angle();
    alinhado = false;
}

void StateBackOff::exitActions()
{
    _robot->moveForward(0);
    _robot->setLastError(0);
    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}
