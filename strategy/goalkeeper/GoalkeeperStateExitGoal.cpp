//
// Created by fhobots on 9/26/19.
//

#include "GoalkeeperStateExitGoal.hpp"


GoalkeeperStateExitGoal::GoalkeeperStateExitGoal(Robot *robot) : State("exit"), _robot(robot) {

}

GoalkeeperStateExitGoal::~GoalkeeperStateExitGoal() {

}

void GoalkeeperStateExitGoal::doActions() {
    Vector2D destination = Global::areaToDeffend.getCenter();
    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(),  _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string GoalkeeperStateExitGoal::checkConditions() {

    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(WorldModel::isNearOf(_robot->getPosition(),_robot->getObjective()))
        return "seeking";

    return "";
}

void GoalkeeperStateExitGoal::entryActions() {
    
}

void GoalkeeperStateExitGoal::exitActions() {

}

