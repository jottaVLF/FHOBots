//
// Created by wolves on 19/09/19.
//

#include "AttackerStateSpinning.hpp"
#include "../../Global.hpp"

AttackerStateSpinning::AttackerStateSpinning(Robot *robot) : State("spinning"), _robot(robot) {

}

AttackerStateSpinning::~AttackerStateSpinning() {



}

void AttackerStateSpinning::doActions() {
    if(_robot->getPosition().y < Global::fieldRect.height/2 && WorldModel::isDeffenseFieldOnLeft()){
        _robot->spinCounterClockWise(80);
        Global::communication->writeMessage(_robot->getPosMessage(), 80, 80, false,true);
    }else if(_robot->getPosition().y > Global::fieldRect.height/2 && WorldModel::isDeffenseFieldOnLeft()){
        _robot->spinClockWise(80);
        Global::communication->writeMessage(_robot->getPosMessage(), 80, 80, true,false);
    }
}

std::string AttackerStateSpinning::checkConditions() {
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

     if(!WorldModel::isNearOf(_robot->getPosition(), Global::ball))
        return "seeking";

    return "";
}

void AttackerStateSpinning::entryActions() {

}

void AttackerStateSpinning::exitActions() {
    _robot->moveForward(0);
}
