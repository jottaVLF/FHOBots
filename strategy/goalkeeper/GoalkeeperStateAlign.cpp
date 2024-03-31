#include "GoalkeeperStateAlign.hpp"
#include <cmath>
#include "../../Global.hpp"

GoalkeeperStateAlign::GoalkeeperStateAlign(Robot *robot) : State("align"), _robot(robot)
{}

GoalkeeperStateAlign::~GoalkeeperStateAlign()
{}

void GoalkeeperStateAlign::doActions()
{
    double angleError = ((Global::ball - _robot->getPosition())||_robot->getOrientation());
    int pwmRight, pwmLeft;

    if(angleError > 0.35)
    {
        pwmRight = 50;
        pwmLeft = 50+129;

    }
    else if(angleError < - 0.35)
    {
        pwmRight = 50+129;
        pwmLeft = 50;
    }
    else
    {
        pwmLeft = 0;
        pwmRight = 0;
    }

    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}

std::string GoalkeeperStateAlign::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(Global::robotNearBall(_robot->getPosition(), 12) && Global::isInsideOwnArea(Global::ballPos))
        return "kicking";

    Vector2D comp(1, 0);
    Vector2D destination = Global::ball - _robot->getPosition();

    if(((_robot->getOrientation()&&comp) > -8 && destination.y > 10) || ((_robot->getOrientation()&&comp) < 8 && destination.y < -10))
        return "turnaround";

    return "";
}

void GoalkeeperStateAlign::entryActions()
{
    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}

void GoalkeeperStateAlign::exitActions()
{

}