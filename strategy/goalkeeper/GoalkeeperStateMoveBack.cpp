//
// Created by fhobots on 8/22/19.
//

#include "GoalkeeperStateMoveBack.hpp"

GoalkeeperStateMoveBack::GoalkeeperStateMoveBack(Robot *robot) : State("moveback"), _robot(robot){

}

GoalkeeperStateMoveBack::~GoalkeeperStateMoveBack() {

}

void GoalkeeperStateMoveBack::doActions() {
    Vector2D destination = WorldModel::getGoalKeeperDeffencePosition();
    _robot->moveBackward(0);
    _robot->calculatePwmR(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmRight(),  _robot->getPwmLeft(), true, true);

}

std::string GoalkeeperStateMoveBack::checkConditions() {

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
/*
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
*/
    return "";

}

void GoalkeeperStateMoveBack::entryActions() {
        _robot->setPD(10, 250);
}

void GoalkeeperStateMoveBack::exitActions() {
}

