//
// Created by fhobots on 8/22/19.
//

#include "GoalkeeperStateWaiting.hpp"

#include "../../Global.hpp"
#include <cmath>
#include <iostream>

GoalkeeperStateWaiting::GoalkeeperStateWaiting(Robot *robot) : State("waiting"), _robot(robot){

}

GoalkeeperStateWaiting::~GoalkeeperStateWaiting() {

}

void GoalkeeperStateWaiting::doActions() {

   // std::cout<<_robot->getPosition().x<<" "<<_robot->getPosition().y<<std::endl;

   Vector2D dist = Global::ballPos - _robot->getPosition();
   double angle = dist.angleBetween(_robot->getOrientarion());
   double prod = dist&&_robot->getOrientarion();
   
        Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}

std::string GoalkeeperStateWaiting::checkConditions() {
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(Global::isInsideOwnGoal(_robot->getPosition()))
        return "exit";

    if(waitingToReturn())
        return "return";

    if(waitingToSpinning())
        return "spinning";

    if(waitingToKicking())
        return "kicking";

    if(waitingToMoveBack())
        return "moveback";

    if(waitingMoveToForward())
        return "moveforward";

   // if(waitingToKicking())
     //   return "kicking";*/

    return "";
}

void GoalkeeperStateWaiting::entryActions() {

   const Area rect = Global::areaToDeffend;

    valueX = rect.x;

    double halfboardY = (Global::fieldRect.y + Global::fieldRect.y + Global::fieldRect.height) / 2.0;

    if(_robot->getPosition().y <= halfboardY)
        _robot->setPD(30, 2.5); //left side
    else
        _robot->setPD(50, 2.5); //right side

}

void GoalkeeperStateWaiting::exitActions() {

}

bool GoalkeeperStateWaiting::waitingToMoveBack() {

    if(abs(_robot->getOrientarion().y) > 0.5)
    {
        if((Global::eAreaDeffend == AREA_DEFFEND_LEFT && _robot->getPosition().x <= Global::areaToDeffend.x + Global::areaToDeffend.y) ||
           (Global::eAreaDeffend == AREA_DEFFEND_RIGHT && _robot->getPosition().x >= Global::areaToDeffend.x)) {
            if (Global::ball.y - _robot->getPosition().y >= Global::frameCentimetersConstant * 7 &&
                waitingToMoveBackLimit(1))
                return true;
            if (Global::ball.y - _robot->getPosition().y <= Global::frameCentimetersConstant * -7 &&
                waitingToMoveBackLimit(0))
                return true;
        }
    }

    return false;
}

bool GoalkeeperStateWaiting::waitingToMoveBackLimit(int kind) {

    Area goalDefend = Global::areaGoalDeffend;

    if(kind == 0 && _robot->getOrientarion().y >= 0 && _robot->getPosition().y > goalDefend.y)
        return true;
    else if(kind == 1 && _robot->getOrientarion().y < 0 && _robot->getPosition().y < goalDefend.y + goalDefend.height)
        return true;

    return false;
}

bool GoalkeeperStateWaiting::waitingMoveToForward() {

    if(abs(_robot->getOrientarion().y) > 0.5)
    {
        if((Global::eAreaDeffend == AREA_DEFFEND_LEFT && _robot->getPosition().x <= Global::areaToDeffend.x + Global::areaToDeffend.y) ||
                (Global::eAreaDeffend == AREA_DEFFEND_RIGHT && _robot->getPosition().x >= Global::areaToDeffend.x)) {
            if (Global::ball.y - _robot->getPosition().y >= Global::frameCentimetersConstant * 6 &&
                waitingMoveToForwardLimit(0) && _robot->getOrientarion().y >= 0)
                return true;
            if (Global::ball.y - _robot->getPosition().y <= Global::frameCentimetersConstant * -6 &&
                waitingMoveToForwardLimit(1) && _robot->getOrientarion().y <= 0)
                return true;
        }
    }

    return false;
}

bool GoalkeeperStateWaiting::waitingMoveToForwardLimit(int kind) {

    Area goalDeffend = Global::areaGoalDeffend;

    if(kind == 0 && _robot->getOrientarion().y >= 0 && _robot->getPosition().y < goalDeffend.y + goalDeffend.height)
        return true;
    else if(kind == 1 && _robot->getOrientarion().y < 0 && _robot->getPosition().y > goalDeffend.y)
        return true;
    return false;
}

bool GoalkeeperStateWaiting::waitingToSpinning() {

    const double magnitude = (Global::ball - _robot->getPosition()).magnitude();
    const double magnitudeReal = (Global::ballPos - _robot->getPosition()).magnitude();
    return (magnitude < Global::frameCentimetersConstant * 13 || magnitudeReal < Global::frameCentimetersConstant * 8 ||
        Global::ball.x == -10);
}

bool GoalkeeperStateWaiting::waitingToKicking() {

    if(Global::isInsideOwnArea(Global::ballPos))
        return true;

    return false;
}

bool GoalkeeperStateWaiting::waitingToReturn() {
    if(!Global::isInsideOwnArea(_robot->getPosition()))
        return true;

    return false;
}
