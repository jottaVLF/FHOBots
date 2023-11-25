//
// Created by fhobots on 8/22/19.
//

#include "GoalkeeperMoveForward.hpp"

#include "../../Global.hpp"
#include <cmath>
#include <iostream>

GoalkeeperMoveForward::GoalkeeperMoveForward(Robot *robot) : State("moveforward"), _robot(robot) {

}

GoalkeeperMoveForward::~GoalkeeperMoveForward() {

}

void GoalkeeperMoveForward::doActions() {

    Vector2D destination;

    double halfBoardy = (Global::fieldRect.y + Global::fieldRect.height + Global::fieldRect.y)/2.0;

    if(Global::ball.y > halfBoardy && _robot->getOrientarion().y >= 0)
        destination = {valueX, double(Global::fieldRect.y + Global::fieldRect.height)};
    else if(Global::ball.y < halfBoardy && _robot->getOrientarion().y < 0)
        destination = {valueX, 0};
    else
        destination = {valueX, Global::ball.y};

    _robot->calculatePwm(destination);


    Global::posPointGoalkeep = destination;

    if(_robot->getPwmLeft() % 2 == 0)
        _robot->setPwmLeft(_robot->getPwmLeft() + 1);
    if(_robot->getPwmRight() %2 == 0)
        _robot->setPwmRight(_robot->getPwmRight() + 1);

    Global::communication->writeMessage(_robot->getPosMessage(),  _robot->getPwmLeft(), _robot->getPwmRight() + 10);
}

std::string GoalkeeperMoveForward::checkConditions() {
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(forwardToBack())
        return "moveback";

    if(Global::isInsideOwnGoal(_robot->getPosition()))
        return "exit";

    if(forwardToWaiting())
        return "waiting";

    if(Global::ball.x == -10)
        return "spinning";

    return "";
}

void GoalkeeperMoveForward::entryActions() {
    Area rect = Global::areaToDeffend;
    valueX = (rect.x + rect.width+rect.x)/2.0 - 10;
    
    double halfboardY = (Global::fieldRect.y + Global::fieldRect.y + Global::fieldRect.height) / 2.0;

    _robot->setPD(40, 100);
}

void GoalkeeperMoveForward::exitActions() {

}

bool GoalkeeperMoveForward::forwardToWaiting() {

    if(abs(Global::ball.y - _robot->getPosition().y) < Global::frameCentimetersConstant*5.5 || forwardToWaitingLimit())
        return true;

    return false;
}

bool GoalkeeperMoveForward::forwardToWaitingLimit() {

    Area goalDefend = Global::areaGoalDeffend;

    if(_robot->getOrientarion().y >= 0 && _robot->getPosition().y > goalDefend.y + goalDefend.height)
        return true;
    else if(_robot->getOrientarion().y < 0 && _robot->getPosition().y < goalDefend.y)
        return true;

    return false;
}

bool GoalkeeperMoveForward::forwardToBack() {

    if(abs(_robot->getOrientarion().y) > 0.5){
        if(Global::ball.y - _robot->getPosition().y >= Global::frameCentimetersConstant*8 && _robot->getOrientarion().y <= 0)
            return true;
        if(Global::ball.y - _robot->getPosition().y <= -1 * Global::frameCentimetersConstant*8 && _robot->getOrientarion().y >= 0)
            return true;
    }

    return false;
}
