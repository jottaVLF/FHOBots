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
    if(Global::eAreaDeffend == AREA_DEFFEND_LEFT) {
        if ((Global::ball - _robot->getPosition()).y > 0)
            Global::communication->writeMessage(_robot->getPosMessage(), 21, 255);
        else
            Global::communication->writeMessage(_robot->getPosMessage(), 20, 255);
    }
    else {
        if ((Global::ball - _robot->getPosition()).y > 0)
            Global::communication->writeMessage(_robot->getPosMessage(), 20, 255);
        else
            Global::communication->writeMessage(_robot->getPosMessage(), 21, 255);
    }

}

std::string GoalkeeperStateSpinning::checkConditions() {
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(spinningToWaiting())
        return "waiting";

    if(Global::isInsideOwnArea(Global::ballPos) && Global::ballInAreaCounter == 50)
        return "kicking";

    return "";
}

void GoalkeeperStateSpinning::entryActions() {

}

void GoalkeeperStateSpinning::exitActions() {

    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);

}

bool GoalkeeperStateSpinning::spinningToWaiting() {
    const double magnitude = (Global::ballPos - _robot->getPosition()).magnitude();
    return (magnitude > Global::frameCentimetersConstant*15 && Global::ball.x >= 0);
}
