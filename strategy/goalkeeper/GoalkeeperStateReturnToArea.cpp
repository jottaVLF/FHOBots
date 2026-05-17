//
// Created by fhobots on 9/26/19.
//

#include "GoalkeeperStateReturnToArea.hpp"
#include "../../Global.hpp"
#include <iostream>

GoalkeeperStateReturnToArea::GoalkeeperStateReturnToArea(Robot *robot) : State("return"), _robot(robot) {

}

GoalkeeperStateReturnToArea::~GoalkeeperStateReturnToArea() {

}

void GoalkeeperStateReturnToArea::doActions() 
{
        Vector2D destination(Global::areaToDeffend.getCenter());        
        _robot->calculatePwm(destination);
        Global::communication->writeMessage(_robot->getPosMessage(),  _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string GoalkeeperStateReturnToArea::checkConditions() {

    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(WorldModel::isInDeffenseArea(_robot->getPosition()))
        return "seeking";
    if(WorldModel::isStuckAtDeffenseGoal(_robot->getPosition(), _robot->getOrientation()))
        return "exit";
    return "";
}

void GoalkeeperStateReturnToArea::entryActions() {
        _robot->setPD(10, 0);
        _robot->setBasePwmValue(35);
        _robot->setMaxPwm(60);
}

void GoalkeeperStateReturnToArea::exitActions() {

}
