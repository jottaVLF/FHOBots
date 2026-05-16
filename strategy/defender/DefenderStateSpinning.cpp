#include "DefenderStateSpinning.hpp"
#include "../../Global.hpp"

DefenderStateSpinning::DefenderStateSpinning(Robot *robot) : State("spinning"), _robot(robot) {

}

DefenderStateSpinning::~DefenderStateSpinning() {

}

void DefenderStateSpinning::doActions() {
    this->timer++;
    if(_robot->getPosition().y < Global::fieldRect.height/2 && WorldModel::isDeffenseFieldOnLeft()){
        _robot->spinCounterClockWise(80);
        Global::communication->writeMessage(_robot->getPosMessage(), 180, 180, true,false);
    }else if(_robot->getPosition().y > Global::fieldRect.height/2 && WorldModel::isDeffenseFieldOnLeft()){
        _robot->spinClockWise(80);
        Global::communication->writeMessage(_robot->getPosMessage(), 180, 180, false,true);
    }
}

std::string DefenderStateSpinning::checkConditions() {
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(this->timer > 500){
        return "seeking";
    }

   /*  if(!WorldModel::isNearOf(_robot->getPosition(), Global::ball))
        return "seeking";
    */
    return "";
}

void DefenderStateSpinning::entryActions() {
    this->timer = 0;
}

void DefenderStateSpinning::exitActions() {
    _robot->moveForward(0);
}
