//
// Created by fhobots on 9/26/19.
//

#include "GoalkeeperStateExitGoal.hpp"
#include <iostream>

GoalkeeperStateExitGoal::GoalkeeperStateExitGoal(Robot *robot) : State("exit"), _robot(robot) {

}

GoalkeeperStateExitGoal::~GoalkeeperStateExitGoal() {

}
bool GoalkeeperStateExitGoal::checkGoalWalls(){
    if(_robot->getOrientation().y < 0 && 
    _robot->getPosition().y > Global::areaGoalDeffend.getCenter().y){
         return true;
    }
    if(_robot->getOrientation().y > 0 && 
    _robot->getPosition().y < Global::areaGoalDeffend.getCenter().y){
         return true;
    }
    return false;
   
}
void GoalkeeperStateExitGoal::doActions() {
    Vector2D destination = Global::areaToDeffend.getCenter();
    Vector2D robotToGoalCenter = Global::areaGoalDeffend.getCenter()-_robot->getPosition();
    Vector2D robotToAreaCenter = Global::areaToDeffend.getCenter()-_robot->getPosition();
    Vector2D goalUpperWall = Global::areaGoalDeffend.y;
    Vector2D goalLowerWall = Global::areaGoalDeffend.y + Global::areaGoalDeffend.height;

    _robot->setObjective(destination);
    if(!WorldModel::isAlignedWith(_robot->getOrientation(),robotToAreaCenter)&&
    WorldModel::isNearOf(_robot->getPosition(),Global::areaGoalDeffend.getCenter())){
        
        _robot->moveBackward(50);
        Global::communication->writeMessage(_robot->getPosMessage(),_robot->getPwmLeft(), _robot->getPwmRight(), true, true);
    }

    
    else if(WorldModel::isAlignedWith(_robot->getOrientation(),robotToGoalCenter)||
    checkGoalWalls()){
        _robot->moveForward(0);
        _robot->calculatePwmUnivector(destination);
        Global::communication->writeMessage(_robot->getPosMessage(),  _robot->getPwmLeft(), _robot->getPwmRight());
    }
    
    else if(!WorldModel::isNearOf(_robot->getPosition(),Global::areaGoalDeffend.getCenter())){
        _robot->moveBackward(50);
        Global::communication->writeMessage(_robot->getPosMessage(),_robot->getPwmLeft(), _robot->getPwmRight(), true, true);
    }
}

std::string GoalkeeperStateExitGoal::checkConditions() {

    if(Global::bufferKeyboard == (int)'p')
        return "idle";
    if(checkGoalWalls()){
        return "return";
    }
    if(WorldModel::isNearOf(_robot->getPosition(),_robot->getObjective())){
        return "align";
}
    return "";
}

void GoalkeeperStateExitGoal::entryActions() {
    
}

void GoalkeeperStateExitGoal::exitActions() {

}
