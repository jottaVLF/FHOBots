#include "GoalkeeperStateWaiting.hpp"


GoalkeeperStateWaiting::GoalkeeperStateWaiting(Robot *robot) : State("waiting"), _robot(robot){

}

GoalkeeperStateWaiting::~GoalkeeperStateWaiting() {

}

void GoalkeeperStateWaiting::doActions() {
    Vector2D destination = WorldModel::getGoalKeeperDeffencePosition();
    _robot->setObjective(destination);
    _robot->setPwmLeft(0);
    _robot->setPwmRight(0);
    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}

std::string GoalkeeperStateWaiting::checkConditions() {
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(!WorldModel::isNearOf(_robot->getPosition(), _robot->getObjective()))
        return "seeking";
/*
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
