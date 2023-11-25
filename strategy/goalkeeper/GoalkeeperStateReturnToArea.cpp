//
// Created by fhobots on 9/26/19.
//

#include "GoalkeeperStateReturnToArea.hpp"
#include "../../Global.hpp"
#include <iostream>

GoalkeeperStateReturnToArea::GoalkeeperStateReturnToArea(Robot *robot) : State("return"), _robot(robot) {

}

GoalkeeperStateReturnToArea::~GoalkeeperStateReturnToArea() {

}

void GoalkeeperStateReturnToArea::doActions() {

        Vector2D destination;

        destination.set(Global::areaToDeffend.x, Global::areaToDeffend.y);
        Vector2D oriAux = destination - _robot->getPosition();
        std::cout<<"Destinaton: (" << destination.x << ", " << destination.y << ")" << std::endl;
        
        std::cout<<(_robot->getOrientarion()||oriAux)<< " " << oriAux.magnitude() << std::endl;
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

            Global::communication->writeMessage(_robot->getPosMessage(),  _robot->getPwmLeft(), _robot->getPwmRight() + 10);
        }
}

std::string GoalkeeperStateReturnToArea::checkConditions() {

    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(returnToWaiting())
        return "waiting";

    return "";
}

void GoalkeeperStateReturnToArea::entryActions() {
    valueX = (Global::areaToDeffend.x + Global::areaToDeffend.width + Global::areaToDeffend.x) / 2.0;
    valueY = (Global::areaToDeffend.y + Global::areaToDeffend.y + Global::areaToDeffend.height)/2.0;
    alinhado = false;
}

void GoalkeeperStateReturnToArea::exitActions() {

}

bool GoalkeeperStateReturnToArea::returnToWaiting() {

    if(Global::isInsideOwnArea(_robot->getPosition()))
        return true;

    return false;
}
