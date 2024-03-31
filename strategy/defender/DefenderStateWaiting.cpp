#include "DefenderStateWaiting.hpp"
#include "../../Global.hpp"

DefenderStateWaiting::DefenderStateWaiting(Robot* robot) : State("waiting"), _robot(robot)
{}

DefenderStateWaiting::~DefenderStateWaiting()
{}

void DefenderStateWaiting::doActions()
{
    Vector2D destination(_posFixedX, Global::ball.y);

    if(Global::eAreaDeffend == AREA_DEFFEND_LEFT)
        if(Global::ballPos.x < Global::fieldRect.width/2.0)
            if(Global::attacker.getPosition().x > Global::fieldRect.width/2.0)
                contador++;
            else
                contador = 0;

        else
        if(Global::ballPos.x > Global::fieldRect.width/2.0)
            if(Global::attacker.getPosition().x < Global::fieldRect.width/2.0)
                contador++;
            else
                contador = 0;


    if(contador >= 50)
        _robot->forceSeeking = true;

    destination.set(Global::attacker.getPosition().x - Global::frameCentimetersConstant * 40, Global::attacker.getPosition().y - 40 * Global::frameCentimetersConstant);
    _robot->calculatePwm(destination);

    if(destination.x < Global::fieldRect.x && destination.y < Global::fieldRect.y)
        destination.set(Global::attacker.getPosition().x + 40 * Global::frameCentimetersConstant, Global::attacker.getPosition().y + 40 * Global::frameCentimetersConstant);
    else if(destination.x < Global::fieldRect.x)
        destination.set(Global::attacker.getPosition().x + 40 * Global::frameCentimetersConstant, Global::attacker.getPosition().y - 40 * Global::frameCentimetersConstant);
    else if(destination.y < Global::fieldRect.y)
        destination.set(Global::attacker.getPosition().x - 40 * Global::frameCentimetersConstant, Global::attacker.getPosition().y + 40 * Global::frameCentimetersConstant);

    //_robot->setPwmRight(Global::pwmRightDef(_robot->getPwmRight()));
    Global::posPointDeffend = destination;
    if(_robot->unchangedPosCounter > 80) {
        Global::communication->writeMessage(_robot->getPosMessage(), 3, 50);
        if(_robot->unchangedPosCounter > 90) {
            _robot->unchangedPosCounter = 0;
            Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
        }
    }
    else if(Global::isInsideOwnArea(Global::ball) || Global::isInsideOwnArea(Global::ballPos)) {
        Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
        _robot->unchangedPosCounter = 0;
    }
    else if((_robot->getPosition() - destination).magnitude() <= 10 * Global::frameCentimetersConstant)
    {
        Global::communication->writeMessage(_robot->getPosMessage(), 2, 65);
    }
    else
        Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string DefenderStateWaiting::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";


    Vector2D posRobot = _robot->getPosition();
    Vector2D oriRobot = _robot->getOrientation(); /// Orientação do Robô
    Vector2D comp;

    if(Global::ball.x == -10)
        return "spinning";

    if(Global::isInsideOwnArea(_robot->getPosition()))
        return "exit";

    if(Global::robotNearRobot(_robot))
        return "backoff";

    comp.set(1.0, 0.0);
    if(posRobot.y < Global::frameCentimetersConstant * 10 && (oriRobot&&comp) > 0.50)
        return "backoff";

    comp.set(-1.0, 0.0);
    if(posRobot.y + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.height - Global::frameCentimetersConstant * 10
       && (oriRobot&&comp) > 0.50)
        return "backoff";

    comp.set(0.0, -1.0);
    if(posRobot.x < Global::frameCentimetersConstant * 20 && (oriRobot&&comp) > 0.50)
        return "backoff";

    comp.set(0.0, 1.0);
    if(posRobot.x + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.width - Global::frameCentimetersConstant * 20
       && (oriRobot&&comp) > 0.50)
        return "backoff";

    if(!((Global::attacker.getPosition() - Global::ball).magnitude() <= (Global::deffender.getPosition() - Global::ball).magnitude()) || _robot->forceSeeking)
        return "seeking";

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
