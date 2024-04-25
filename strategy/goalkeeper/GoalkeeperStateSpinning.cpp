//
// Created by fhobots on 8/22/19.
//

#include "GoalkeeperStateSpinning.hpp"

#include "../../Global.hpp"

GoalkeeperStateSpinning::GoalkeeperStateSpinning(Robot *robot) : State("spinning"), _robot(robot) {

}

GoalkeeperStateSpinning::~GoalkeeperStateSpinning() {

}

void GoalkeeperStateSpinning::doActions() {
    if(WorldModel::isDeffenseFieldOnLeft()) {
        if ((Global::ball - _robot->getPosition()).y > 0)
            _robot->spinClockWise(80);
        else
            _robot->spinCounterClockWise(80);
    }
    else {
        if ((Global::ball - _robot->getPosition()).y > 0)
            _robot->spinCounterClockWise(80);
        else
            _robot->spinClockWise(80);
    }
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight(), _robot->reverseLeft, _robot->reverseRight);
}

std::string GoalkeeperStateSpinning::checkConditions() {
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(!WorldModel::isInsideDeffenseArea(Global::ball) || !WorldModel::isNearOf(_robot->getPosition(), Global::ball))
        return "seeking";

    return "";
}

void GoalkeeperStateSpinning::entryActions() {
    _robot->moveForward(0);
}

void GoalkeeperStateSpinning::exitActions() {
    _robot->moveForward(0);
    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}
