#include "AttackerStateWaiting.hpp"
#include "../../Global.hpp"
#include "iostream"

AttackerStateWaiting::AttackerStateWaiting(Robot* robot) : State("waiting"), _robot(robot)
{}

AttackerStateWaiting::~AttackerStateWaiting()
{}

void AttackerStateWaiting::doActions()
{

    Vector2D destination;
  
    destination.set(Global::deffender.getPosition().x - 20 * Global::frameCentimetersConstant, Global::deffender.getPosition().y + 20 * Global::frameCentimetersConstant);

    if(destination.x < Global::fieldRect.x && destination.y < Global::fieldRect.y)
        destination.set(Global::deffender.getPosition().x + 20 * Global::frameCentimetersConstant, Global::deffender.getPosition().y - 20 * Global::frameCentimetersConstant);
    else if(destination.x < Global::fieldRect.x)
        destination.set(Global::deffender.getPosition().x + 20 * Global::frameCentimetersConstant, Global::deffender.getPosition().y - 20 * Global::frameCentimetersConstant);
    else if(destination.y < Global::fieldRect.y)
        destination.set(Global::deffender.getPosition().x - 20 * Global::frameCentimetersConstant, Global::deffender.getPosition().y + 20 * Global::frameCentimetersConstant);

    _robot->calculatePwm(destination);
    Global::posPointAttack = destination;
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), Global::pwmComp1(_robot->getPwmRight()));

}

std::string AttackerStateWaiting::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(Global::robotNearRobot(_robot))
        return "backoff";

    Vector2D posRobot = _robot->getPosition();
    Vector2D oriRrobot = _robot->getOrientation(); 
    Vector2D comp;

    comp.set(1.0, 0.0);
    if(posRobot.y < Global::frameCentimetersConstant * 10 && (oriRrobot&&comp) > 0)
        return "backoff";

    comp.set(-1.0, 0.0);
    if(posRobot.y + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.height - Global::frameCentimetersConstant * 10
       && (oriRrobot&&comp) > 0)
        return "backoff";

    comp.set(0.0, -1.0);
    if(posRobot.x < Global::frameCentimetersConstant * 20 && (oriRrobot&&comp) > 0
       && Global::eAreaDeffend == AREA_DEFFEND_LEFT)
        return "backoff";

    comp.set(0.0, 1.0);
    if(posRobot.x + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.width - Global::frameCentimetersConstant * 20
       && (oriRrobot&&comp) > 0 && Global::eAreaDeffend == AREA_DEFFEND_RIGHT)
        return "backoff";

    if(!((Global::attacker.getPosition() - Global::ball).magnitude() > (Global::deffender.getPosition() - Global::ball).magnitude()))
        return "seeking";

    return "";
}

void AttackerStateWaiting::entryActions()
{
    _robot->setPD(110, 200.85);
    _robot->setBasePwmValue(180);
    _robot->setMaxPwm(255);
    _robot->lastPos = _robot->getPosition();
    _robot->lastOri = _robot->getOrientation();
}

void AttackerStateWaiting::exitActions()
{
    Vector2D robotToDestiny = Global::ball - _robot->getPosition();
    _robot->setLastError((_robot->getOrientation()||robotToDestiny));
}