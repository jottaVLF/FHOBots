#include "StateBackOff.hpp"
#include "../../Global.hpp"

StateBackOff::StateBackOff(Robot* robot) : State("backoff"), _robot(robot), _side(0)
{}

StateBackOff::~StateBackOff()
{}

void StateBackOff::doActions()
{
    Vector2D destination = Global::ball - _robot->getPosition();
    if(Global::robotNearRobot(_robot))
    {
        Global::communication->writeMessage(_robot->getPosMessage(), 2, 80);
        newAngle = _robot->getOrientation().angle();
    }
    else{
        Vector2D destination;
        destination.set(Global::ball.x, Global::ball.y);
        Vector2D oriAux = destination - _robot->getPosition();
        if(abs(_robot->getOrientation()||oriAux) <= M_PI/8) {
            Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
            alinhado = true;
        }
        if((_robot->getOrientation()||oriAux) >= M_PI/4)
            Global::communication->writeMessage(_robot->getPosMessage(), 2, 50);
        else if((_robot->getOrientation()||oriAux) <= -M_PI/4){
            Global::communication->writeMessage(_robot->getPosMessage(), 3, 50);
        }
    }

}

std::string StateBackOff::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    Vector2D posRobot = _robot->getPosition();
    
    if(Global::ball.x == -10)
        return "spinning";

    if(Global::robotNearRobot(_robot))
        return "";

    return "seeking";
}

void StateBackOff::entryActions()
{
    _robot->setPwmRight(0);
    _robot->setPwmLeft(0);
    newAngle = _robot->getOrientation().angle();
    alinhado = false;
    //angle = _robot->getOrientation().angle();
}

void StateBackOff::exitActions()
{
    _robot->setLastError(0);
    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}
