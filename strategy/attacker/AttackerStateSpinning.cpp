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

    if(Global::eAreaDeffend == AREA_DEFFEND_LEFT)
    {
        if(_robot->getPosition().y <= (Global::fieldRect.y + Global::fieldRect.height) / 2.0)
        {
            Global::communication->writeMessage(_robot->getPosMessage(), 2, 255); //horario
        }
        else{
            Global::communication->writeMessage(_robot->getPosMessage(), 3, 255); //anti
        }
    }
    else
    {
        if(_robot->getPosition().y <= (Global::fieldRect.y + Global::fieldRect.height) / 2.0)
        {
            Global::communication->writeMessage(_robot->getPosMessage(), 3, 255); // anti
        }
        else{
            Global::communication->writeMessage(_robot->getPosMessage(), 2, 255); // horario
        }
    }


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
