#include "GoalkeeperStateKicking.hpp"
#include "../../Global.hpp"
#include <iostream>
#include <iomanip>

GoalkeeperStateKicking::GoalkeeperStateKicking(Robot *robot) : State("kicking"), _robot(robot)
{}

GoalkeeperStateKicking::~GoalkeeperStateKicking()
{}

void GoalkeeperStateKicking::doActions()
{
    Vector2D destination = Global::ballPos - _robot->getPosition();

    if((destination.y < 0 && _robot->getOrientarion().y > 0) || (destination.y > 0 && _robot->getOrientarion().y < 0))
    {
        //moveback
        destination = Global::ballPos;
        _robot->calculatePwmR(destination);

        if(_robot->getPwmLeft() % 2 != 0)
            _robot->setPwmLeft(_robot->getPwmLeft() - 1);

        if(_robot->getPwmRight() % 2 != 0)
            _robot->setPwmRight(_robot->getPwmRight() - 1);

    }
    else{
        destination = Global::ballPos;
        _robot->calculatePwm(destination);

        if(_robot->getPwmLeft() % 2 == 0)
            _robot->setPwmLeft(_robot->getPwmLeft() + 1);
        if(_robot->getPwmRight() %2 == 0)
            _robot->setPwmRight(_robot->getPwmRight() + 1);

    }

    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight() + 10);
}

std::string GoalkeeperStateKicking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(kickingToWaiting())
        return "waiting";

    if(Global::ball.x == -10)
        return "spinning";
    return "";
}

void GoalkeeperStateKicking::entryActions()
{
    _robot->setBasePwmValue(95);
    _robot->setPD(36., 80.);
}

void GoalkeeperStateKicking::exitActions()
{
    ///Nada
}

bool GoalkeeperStateKicking::kickingToWaiting() {
    if(!Global::isInsideOwnArea(Global::ballPos))
        return true;

    return false;
}