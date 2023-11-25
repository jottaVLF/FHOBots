//
// Created by fhobots on 9/26/19.
//

#include "GoalkeeperStateExitGoal.hpp"
#include "../../Global.hpp"

GoalkeeperStateExitGoal::GoalkeeperStateExitGoal(Robot *robot) : State("exit"), _robot(robot) {

}

GoalkeeperStateExitGoal::~GoalkeeperStateExitGoal() {

}

void GoalkeeperStateExitGoal::doActions() {

    Vector2D destination;
    destination.set(valueX, valueY);
    Vector2D oriAux = destination - _robot->getPosition();
    ///std::cout<<(_robot->getOrientarion()||oriAux)<<std::endl;
    if((_robot->getOrientarion()||oriAux) >= M_PI/4)
        Global::communication->writeMessage(_robot->getPosMessage(), 51, 50);
    else if((_robot->getOrientarion()||oriAux) <= -M_PI/4){
        Global::communication->writeMessage(_robot->getPosMessage(), 50, 51);
    }
    else{
        if(!alinhado)
            Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
        alinhado = true;
        _robot->calculatePwm(destination);

        if(_robot->getPwmLeft() % 2 == 0)
            _robot->setPwmLeft(_robot->getPwmLeft() + 1);
        if(_robot->getPwmRight() %2 == 0)
            _robot->setPwmRight(_robot->getPwmRight() + 1);

        // std::cout<<_robot->getPwmLeft()<<" "<<_robot->getPwmRight()<<std::endl;
        Global::communication->writeMessage(_robot->getPosMessage(),  _robot->getPwmLeft(), _robot->getPwmRight() + 10);
    }
}

std::string GoalkeeperStateExitGoal::checkConditions() {

    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(exitgoalToWaiting())
        return "waiting";

    return "";
}

void GoalkeeperStateExitGoal::entryActions() {
    valueX = (Global::areaToDeffend.x + Global::areaToDeffend.width + Global::areaToDeffend.x) / 2.0;
    if(Global::ballPos.y < Global::areaGoalDeffend.y)
        valueY = Global::areaGoalDeffend.y;
    else if(Global::ballPos.y > Global::areaGoalDeffend.y + Global::areaGoalDeffend.height)
        valueY = Global::areaGoalDeffend.y + Global::areaGoalDeffend.height;
    else
        valueY = Global::ballPos.y;

    alinhado = false;
}

void GoalkeeperStateExitGoal::exitActions() {

}

bool GoalkeeperStateExitGoal::exitgoalToWaiting() {
    if(!Global::isInsideOwnGoal(_robot->getPosition()))
        return "waiting";
    return false;
}
