//
// Created by fhobots on 10/3/19.
//

#include "StateExitArea.hpp"
#include "../../Global.hpp"

StateExitArea::StateExitArea(Robot *robot) : State("exit"), _robot(robot) {

}

StateExitArea::~StateExitArea() {

}

void StateExitArea::doActions() {

    _robot->calculatePwmUnivector(destination);

    if(_robot->getPosMessage() == 1)
        Global::posPointAttack = destination;
    else
        Global::posPointDeffend = destination;

    if(_robot->unchangedPosCounter > 80) {
        Global::communication->writeMessage(_robot->getPosMessage(), 3, 50);
        if(_robot->unchangedPosCounter > 90) {
            _robot->unchangedPosCounter = 0;
            Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
        }
    }
    else{
        Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
    }

}

std::string StateExitArea::checkConditions() {

    Vector2D posRobot = _robot->getPosition();
    Vector2D oriRobot = _robot->getOrientation(); /// Orientação do Robô
    Vector2D comp;

    Vector2D destination = Global::ball;

    if(Global::ball.x == -10)
        return "spinning";

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


    if(abs(_robot->getPosition().x - destination.x) <= 4*Global::frameCentimetersConstant)
        return "seeking";

    return "";
}

void StateExitArea::entryActions() {
    if(_robot->getPosition().y <= Global::fieldRect.height/2.0)
        valueY = Global::fieldRect.height/4.0;
    else
        valueY = Global::fieldRect.height/2.0 + Global::fieldRect.height/4.0;

    if(_robot->getPosMessage() == 1)
        valueX = Global::fieldRect.width/2.0;
    else
        valueX = Global::fieldRect.width/2.0 + Global::fieldRect.width/4.0;

    destination.set(valueX, valueY);
}

void StateExitArea::exitActions() {

}
