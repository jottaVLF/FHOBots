//
// Created by fhobots on 8/22/19.
//

#include "GoalkeeperStateMoveBack.hpp"
#include "../../Global.hpp"
#include <cmath>
#include <iostream>


GoalkeeperStateMoveBack::GoalkeeperStateMoveBack(Robot *robot) : State("moveback"), _robot(robot){

}

GoalkeeperStateMoveBack::~GoalkeeperStateMoveBack() {

}

void GoalkeeperStateMoveBack::doActions() {

    Vector2D destination;
    double halfBoardy = (Global::fieldRect.y + Global::fieldRect.height + Global::fieldRect.y)/2.0;

    if(Global::ball.y > halfBoardy && _robot->getOrientation().y <= 0)
        destination = {valueX, double(Global::fieldRect.y + Global::fieldRect.height)};
    else if(Global::ball.y < halfBoardy && _robot->getOrientation().y > 0)
        destination = {valueX, 0};
    else
        destination = {valueX, Global::ball.y};

    _robot->calculatePwmR(destination);

    Global::posPointGoalkeep = destination;

    if(_robot->getPwmLeft() % 2 != 0)
        _robot->setPwmLeft(_robot->getPwmLeft() - 1);

    if(_robot->getPwmRight() % 2 != 0)
        _robot->setPwmRight(_robot->getPwmRight() - 1);

    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmRight() + 10,  _robot->getPwmLeft());

}

std::string GoalkeeperStateMoveBack::checkConditions() {

    if(Global::bufferKeyboard == (int)'p')
        return "idle";


    if(backToWaiting()) {
        return "waiting";
    }

    if(Global::isInsideOwnGoal(_robot->getPosition()))
        return "exit";

    if(backToForward()) {
        return "moveforward";
    }

    if(Global::ball.x == -10)
        return "spinning";

    return "";

}

void GoalkeeperStateMoveBack::entryActions() {
    Area rect = Global::areaToDeffend;
    valueX = (rect.x + rect.width + rect.x)/2.0;

    _robot->setPD(70, 105);
}

void GoalkeeperStateMoveBack::exitActions() {
}

bool GoalkeeperStateMoveBack::backToWaiting() {

    if(abs(Global::ball.y - _robot->getPosition().y) < Global::frameCentimetersConstant*5.5 || backToWaitingLimit())
        return true;

    return false;

}

bool GoalkeeperStateMoveBack::backToWaitingLimit() {

    Area goalDefend = Global::areaGoalDeffend;

    if(_robot->getOrientation().y >= 0 && _robot->getPosition().y < goalDefend.y)
        return true;
    else if(_robot->getOrientation().y < 0 && _robot->getPosition().y > goalDefend.y + goalDefend.height)
        return true;

    return false;

}

bool GoalkeeperStateMoveBack::backToForward() {


    //if(abs(_robot->getOrientation().y) > 0.5)
    {
        if(Global::ball.y - _robot->getPosition().y >= 0 && _robot->getOrientation().y >=0)
            return true;
        if(Global::ball.y - _robot->getPosition().y <= 0 && _robot->getOrientation().y <=0)
            return true;
    }

    return false;
}

bool GoalkeeperStateMoveBack::backToBackOff() {

    if(Global::eAreaDeffend == AREA_DEFFEND_LEFT)
        if(_robot->getPosition().x <= Global::areaToDeffend.x)
            return true;
    if(Global::eAreaDeffend == AREA_DEFFEND_RIGHT)
        if(_robot->getPosition().x >= Global::areaGoalDeffend.x)
            return true;

        return false;
}