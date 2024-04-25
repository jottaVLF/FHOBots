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
    //Vector2D destination = Global::ball;
    Vector2D destination = WorldModel::getGoalKeeperDeffencePosition();
    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string GoalkeeperStateSeeking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";
    //return "";
    if(WorldModel::isNearOf(_robot->getPosition(),_robot->getObjective())){
        return "waiting";
    }

    if(!WorldModel::isInDeffenseArea(_robot->getPosition()))
        return "return";
    

    return "";
}

void GoalkeeperStateSeeking::entryActions()
{
    _robot->setPD(10, 250);
    _robot->setBasePwmValue(35);
    _robot->setMaxPwm(80);
}

void GoalkeeperStateSeeking::exitActions()
{}