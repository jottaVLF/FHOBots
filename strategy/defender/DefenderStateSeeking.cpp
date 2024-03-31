#include "DefenderStateSeeking.hpp"
#include "../../Global.hpp"
#include <iostream>

DefenderStateSeeking::DefenderStateSeeking(Robot* robot) : State("seeking"), _robot(robot)
{}

DefenderStateSeeking::~DefenderStateSeeking()
{}

void DefenderStateSeeking::doActions()
{
    Vector2D destination = Global::ball;

    if(_robot->forceSeeking)
        if(abs(Global::attacker.getPosition().x - atkLastX) > 4 * Global::frameCentimetersConstant)
            _robot->forceSeeking = false;

    switch (Global::eAreaAttack)
    {
        case AREA_ATTACK_RIGHT:
            //destination -= _robot->getPosition();
            if(destination.x <= 30) {
                double destinationX = Global::ball.x - 70;
                if(destinationX < Global::fieldRect.x)
                    destinationX = Global::ball.x;
                destination.set(destinationX, destination.y <= 0 ? Global::ball.y + 40 : Global::ball.y - 40);
            }

            else if(destination.x <= 45 && (destination.y > 15 || destination.y < -15))
                destination.set(Global::ball.x, Global::ball.y);

            else if(destination.x > 50)
                destination.set(Global::ball.x - destination.x / 2, Global::ball.y);
            else
                destination = Global::ball;
            break;

        case AREA_ATTACK_LEFT:
            //destination -= _robot->getPosition();
            if(destination.x >= -10)
                destination.set(Global::ball.x + 20, destination.y <= 0 ? Global::ball.y + 40 : Global::ball.y - 40);

            else if(destination.x >= -45 && (destination.y > 15 || destination.y < -15))
                destination.set(Global::ball.x, Global::ball.y);

            else if(destination.x < -50)
                destination.set(Global::ball.x - destination.x / 2,  Global::ball.y);
            else
                destination = Global::ball;
            break;
    }

    if(Global::eAreaDeffend == AREA_DEFFEND_LEFT)
    {
        if(Global::ballPos.x <= Global::areaToDeffend.x + Global::areaToDeffend.width || Global::ballPos.x >= Global::areaToAttack.x)
            destination = Global::ball;
    }
    else{
        if(Global::ballPos.x >= Global::areaToDeffend.x || Global::ballPos.x <= Global::areaToAttack.x + Global::areaToAttack.width)
            destination = Global::ball;
    }

    Global::posPointDeffend = destination;
    _robot->calculatePwm(destination);
 /*   if(_robot->unchangedPosCounter > 80) {
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
    else */
        Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string DefenderStateSeeking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    /*

    Vector2D posRobot = _robot->getPosition();
    Vector2D oriRobot = _robot->getOrientation(); /// Orientação do Robô
    Vector2D comp;

    Vector2D destination = Global::ball;

    Vector2D oriAux = destination - _robot->getPosition();

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
    //std::cout<<(oriRobot&&comp)<<std::endl;
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

    if(Global::robotNearBall(_robot->getPosition(), 10) && Global::eAreaDeffend == AREA_DEFFEND_LEFT && _robot->getOrientation().x > 0)
        return "kicking";

    if(Global::robotNearBall(_robot->getPosition(), 10) && Global::eAreaDeffend == AREA_DEFFEND_RIGHT && _robot->getOrientation().x < 0)
        return "kicking";

    if((Global::attacker.getPosition() - Global::ball).magnitude() <= (Global::deffender.getPosition() - Global::ball).magnitude() && !_robot->forceSeeking)
        return "waiting";
    */
    return "";
}

void DefenderStateSeeking::entryActions()
{
    _robot->setPD(105, 45); ///180.5, -950.35
    _robot->setBasePwmValue(160);
    _robot->setMaxPwm(255);
    atkLastX = Global::attacker.getPosition().x;
}

void DefenderStateSeeking::exitActions()
{
    _robot->forceSeeking = false;
}