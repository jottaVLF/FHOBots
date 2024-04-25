#include "GoalkeeperStateSeeking.hpp"
#include "../../Global.hpp"
#include <cmath>
#include "../../model/Vector2D.hpp"

GoalkeeperStateSeeking::GoalkeeperStateSeeking(Robot *robot) : State("seeking"), _robot(robot)
{}

GoalkeeperStateSeeking::~GoalkeeperStateSeeking()
{}

void GoalkeeperStateSeeking::doActions()
{
    if(!_stopped){
        Vector2D destination = Global::ball;
        _robot->calculatePwm(destination);
        Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
    }
}

std::string GoalkeeperStateSeeking::checkConditions()
{
    Vector2D pointToDeffend  = WorldModel::getGoalKeeperDeffencePosition();
    Vector2D robotToObjective = pointToDeffend - _robot->getPosition();

    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    // Robot is in the area

    if(!WorldModel::isInDeffenseArea(Global::ball) && 
       !WorldModel::isInFrontOf(_robot->getOrientation(), robotToObjective) &&
       WorldModel::isInDeffenseArea(_robot->getPosition()))
        return "moveback";
    
    if(!WorldModel::isInDeffenseArea(Global::ball) && 
       WorldModel::isInFrontOf(_robot->getOrientation(), robotToObjective) &&
       WorldModel::isInDeffenseArea(_robot->getPosition()))
        return "moveforward";

    if(WorldModel::isInDeffenseArea(Global::ball) && WorldModel::isNearOf(_robot->getPosition(),Global::ball))
        return "spinning";

    if(!WorldModel::isInDeffenseArea(Global::ball) && WorldModel::isNearOf(_robot->getPosition(),_robot->getObjective()))
        return "kicking";

    if(!WorldModel::isInDeffenseArea(_robot->getPosition()))
        return "return";
    
    _stopped = false;

    return "";
}

void GoalkeeperStateSeeking::entryActions()
{
    _stopped = true;
    _robot->setPD(10, 250);
    _robot->setBasePwmValue(35);
    _robot->setMaxPwm(80);
}

void GoalkeeperStateSeeking::exitActions()
{}