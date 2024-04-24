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
    Vector2D destination = WorldModel::getGoalKeeperDeffencePosition();
    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string GoalkeeperStateSeeking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(WorldModel::isNearOf(_robot->getPosition(),_robot->getObjective())){
        return "waiting";
    }

    if(!WorldModel::isInDeffenseArea(_robot->getPosition()))
        return "return";
    

    return "";
}

void GoalkeeperStateSeeking::entryActions()
{
    _robot->setBasePwmValue(160);
    _robot->setMaxPwm(255);
    _robot->setPD(108.,210.);
}

void GoalkeeperStateSeeking::exitActions()
{}