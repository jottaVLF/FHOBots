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
    Vector2D destination = WorldModel::getGoalKeeperDeffencePosition();
    _robot->moveForward(0);
    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(),  _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string GoalkeeperMoveForward::checkConditions() {
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(WorldModel::isStuckAtDeffenseGoal(_robot->getPosition(), _robot->getOrientation()))
        return "exit";

    if(WorldModel::isInDeffenseArea(Global::ball) && WorldModel::isNearOf(_robot->getPosition(),Global::ball))
        return "spinning";

    if(WorldModel::isInDeffenseArea(Global::ball) && !WorldModel::isNearOf(_robot->getPosition(),Global::ball))
        return "seeking";

    if(!WorldModel::isInDeffenseArea(Global::ball) && WorldModel::isNearOf(_robot->getPosition(),_robot->getObjective())){
        return "waiting";
    }

    return "";
}

void GoalkeeperMoveForward::entryActions() {
    _robot->setPD(10, 30); ///180.5, -950.35
    _robot->setBasePwmValue(80);
    _robot->setMaxPwm(255);
}

void GoalkeeperMoveForward::exitActions() {
    _robot->moveForward(0);
}


