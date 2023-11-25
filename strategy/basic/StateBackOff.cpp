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
        newAngle = _robot->getOrientarion().angle();
        //if(abs(newAngle - angle) >= M_PI_2)
            //Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
    }
    else{
        Vector2D destination;
        destination.set(Global::ball.x, Global::ball.y);
        Vector2D oriAux = destination - _robot->getPosition();
        if(abs(_robot->getOrientarion()||oriAux) <= M_PI/8) {
            Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
            alinhado = true;
        }
        if((_robot->getOrientarion()||oriAux) >= M_PI/4)
            Global::communication->writeMessage(_robot->getPosMessage(), 2, 50);
        else if((_robot->getOrientarion()||oriAux) <= -M_PI/4){
            Global::communication->writeMessage(_robot->getPosMessage(), 3, 50);
        }
    }

}

std::string StateBackOff::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    Vector2D posRobot = _robot->getPosition();
    Vector2D oriRobot = _robot->getOrientarion(); /// Orientação do Robô
    Vector2D comp;

    if(Global::ball.x == -10)
        return "spinning";

    if(alinhado)
        return "seeking";

    comp.set(1.0, 0.0);
    //printf("1\n");
    if((posRobot.y < Global::frameCentimetersConstant * 8 && (oriRobot&&comp) > -2))
        return "";

    comp.set(-1.0, 0.0);
    //printf("2\n");
    if((posRobot.y + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.height - Global::frameCentimetersConstant * 8
       && (oriRobot&&comp) > -2))
        return "";

    comp.set(0.0, -1.0);
    //printf("3\n");
    if((posRobot.x < Global::frameCentimetersConstant * 20 && (oriRobot&&comp) > -2))
        return "";

    comp.set(0.0, 1.0);
    //printf("4\n");
    if((posRobot.x + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.width - Global::frameCentimetersConstant * 20
       && (oriRobot&&comp) > -2))
        return "";


    if(Global::robotNearRobot(_robot))
        return "";

    return "seeking";
}

void StateBackOff::entryActions()
{
    _robot->setPwmRight(0);
    _robot->setPwmLeft(0);
    newAngle = _robot->getOrientarion().angle();
    alinhado = false;
    //angle = _robot->getOrientarion().angle();
}

void StateBackOff::exitActions()
{
    _robot->setLastError(0);
    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}
