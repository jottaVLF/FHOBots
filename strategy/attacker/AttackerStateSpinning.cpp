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

       Global::communication->writeMessage(_robot->getPosMessage(), 80, 30, true,false);


}

std::string AttackerStateSpinning::checkConditions() {
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(spinningToSeeking())
       return "seeking";

    return "";
}

void AttackerStateSpinning::entryActions() {

}

void AttackerStateSpinning::exitActions() {

}

bool AttackerStateSpinning::spinningToSeeking() {

    if(abs((_robot->getPosition() - Global::ballPos).magnitude()) >= Global::frameCentimetersConstant * 8 && Global::ball.x != -10)
        return true;

    return false;
}
