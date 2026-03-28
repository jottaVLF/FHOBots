#include "GoalkeeperStateWaiting.hpp"


GoalkeeperStateWaiting::GoalkeeperStateWaiting(Robot *robot) : State("waiting"), _robot(robot){

}

GoalkeeperStateWaiting::~GoalkeeperStateWaiting() {

}

void GoalkeeperStateWaiting::doActions() {
    
    if((_ballPos - Global::ball).magnitude() < 10)
        _timer++;
    else{
        _timer = 0;
        _ballPos = Global::ball;
    }

    Vector2D destination = WorldModel::getGoalKeeperDeffencePosition();
    _robot->setObjective(destination);
    _robot->moveForward(0);
    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}

std::string GoalkeeperStateWaiting::checkConditions() {
    Vector2D pointToDeffend  = WorldModel::getGoalKeeperDeffencePosition();
    Vector2D robotToObjective = pointToDeffend - _robot->getPosition();

    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(WorldModel::isBallNearDeffenceArea() && WorldModel::nearstRobotTo(Global::ball) == _robot && _timer > 2000)
        return "kicking";

    if(WorldModel::isStuckAtDeffenseGoal(_robot->getPosition(), _robot->getOrientation()))
        return "exit";

    if(!WorldModel::isInDeffenseArea(Global::ball) && 
       !WorldModel::isInFrontOf(_robot->getOrientation(), robotToObjective) &&
       WorldModel::isInDeffenseArea(_robot->getPosition()) &&
       !WorldModel::isNearOf(_robot->getPosition(), pointToDeffend))
        return "moveback";
    
    if(!WorldModel::isInDeffenseArea(Global::ball) && 
       WorldModel::isInFrontOf(_robot->getOrientation(), robotToObjective) &&
       WorldModel::isInDeffenseArea(_robot->getPosition()) &&
       !WorldModel::isNearOf(_robot->getPosition(), pointToDeffend))
        return "moveforward";

    if(WorldModel::isInDeffenseArea(Global::ball) && !WorldModel::isNearOf(_robot->getPosition(), _robot->getObjective()))
        return "seeking";

    if(WorldModel::isNearOf(_robot->getPosition(), Global::ball) && WorldModel::isInsideDeffenseArea(_robot->getPosition()))
        return "spinning";

    return "";
}

void GoalkeeperStateWaiting::entryActions() {

    _timer = 0;
    _ballPos = Global::ball;
    double halfboardY = (Global::fieldRect.y + Global::fieldRect.y + Global::fieldRect.height) / 2.0;

    if(_robot->getPosition().y <= halfboardY){
        if(Global::isSim){
            // Calibrações para o simulador
            _robot->setPD(30, 2.5); //left side
        }else{
            _robot->setPD(30, 2.5);
        }
    }else{
        if(Global::isSim){
            // Calibrações para o simulador
            _robot->setPD(50, 2.5); //right side
        }else{
            _robot->setPD(50, 2.5);
        }
    }
}

void GoalkeeperStateWaiting::exitActions() {

}
