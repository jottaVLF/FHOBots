#include "GoalkeeperStateRetreating.hpp"
#include "../../Global.hpp"
#include <iostream>
#include <iomanip>
#include <iostream>

GoalkeeperStateRetreating::GoalkeeperStateRetreating(Robot *robot) : State("retreating"), _robot(robot)
{}

GoalkeeperStateRetreating::~GoalkeeperStateRetreating()
{}

void GoalkeeperStateRetreating::doActions()
{

    Vector2D destination(Global::eAreaDeffend == AREA_DEFFEND_LEFT ? (Global::areaGoalDeffend.x + Global::areaGoalDeffend.width) :
                         (Global::areaGoalDeffend.x), Global::centerAreaToDeffend.y);
    Global::posPointGoalkeep = destination;
    if(Global::eAreaDeffend == AREA_DEFFEND_LEFT && (_robot->getPosition().x <= Global::areaToDeffend.x))
    {
        _robot->calculatePwm(destination);
        if(_robot->getOrientation().x > 0)
            Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());

        else
            Global::communication->writeMessage(_robot->getPosMessage(), 45, 45 + 129);

        return;
    }

    if(Global::eAreaDeffend == AREA_DEFFEND_RIGHT && (_robot->getPosition().x >= Global::areaToDeffend.x + Global::areaToDeffend.width))
    {
        _robot->calculatePwm(destination);
        if(_robot->getOrientation().x > 0)
            Global::communication->writeMessage(_robot->getPosMessage(), 45, 45 + 129);

        else
            Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), Global::pwmGFComp(_robot->getPwmRight()));

        return;
    }

    Vector2D oriRobot = _robot->getOrientation();
    Vector2D posRobot = _robot->getPosition();
    Vector2D comp;

    comp.set(0.0, 1.0);
    if(posRobot.x - 3.75 * Global::frameCentimetersConstant < Global::frameCentimetersConstant * 20  && (oriRobot&&comp) > 0)
    {
        if(posRobot.y > Global::centerAreaToDeffend.y)
            Global::communication->writeMessage(_robot->getPosMessage(), 40, 40 + 129);

        else
            Global::communication->writeMessage(_robot->getPosMessage(), 40 + 129, 40);

        return;
    }

    comp.set(0.0, -1.0);
    if(posRobot.x + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.width - Global::frameCentimetersConstant * 20 &&
       (oriRobot&&comp) > 0)
    {
        if(posRobot.y > Global::centerAreaToDeffend.y)
            Global::communication->writeMessage(_robot->getPosMessage(), 45 + 129, 45);
        else
            Global::communication->writeMessage(_robot->getPosMessage(), 45, 45 + 129);

        return;
    }

    _robot->calculatePwmR(destination);
    Global::posPointGoalkeep = destination;
   // _robot->setPwmRight(Global::pwmRightGolR(_robot->getPwmRight()));
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmRight() + 128, Global::pwmGBComp(_robot->getPwmLeft()) + 128);
}

std::string GoalkeeperStateRetreating::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(Global::robotNearRobot(_robot))
    return "backoff";

    if(Global::areaToDeffend.x < Global::areaToAttack.x)
        if (!(Global::areaToDeffend.x + Global::areaToDeffend.width < _robot->getPosition().x ||
              Global::areaToDeffend.y > _robot->getPosition().y ||
              Global::areaToDeffend.y + Global::areaToDeffend.height < _robot->getPosition().y || Global::areaToDeffend.x + 8 > _robot->getPosition().x))
            return "seeking";

    if(Global::areaToDeffend.x > Global::areaToAttack.x)
        if (!(Global::areaToDeffend.x > _robot->getPosition().x || Global::areaToDeffend.y > _robot->getPosition().y ||
              Global::areaToDeffend.y + Global::areaToDeffend.height < _robot->getPosition().y || Global::areaToDeffend.x + Global::areaToDeffend.width - 8 < _robot->getPosition().x))
            return "seeking";

    return "";
}

void GoalkeeperStateRetreating::entryActions()
{
    if(Global::isSim){
        // Calibrações para o simulador
        _robot->setBasePwmValue(50);
        _robot->setPD(32., 120.);
    }else{
        _robot->setBasePwmValue(50);
        _robot->setPD(32., 120.);
    }
}

void GoalkeeperStateRetreating::exitActions()
{
    ///Nada
}

bool GoalkeeperStateRetreating::retreatingToWaiting() {
    ///implementar

    return false;
}