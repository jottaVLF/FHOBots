#include "GoalkeeperStateTurnaround.hpp"
#include "../../Global.hpp"
#include <iostream>

GoalkeeperStateTurnaround::GoalkeeperStateTurnaround(Robot *robot) : State("turnaround"), _robot(robot)
{}

GoalkeeperStateTurnaround::~GoalkeeperStateTurnaround()
{}

void GoalkeeperStateTurnaround::doActions()
{
    Vector2D destination = Global::ball - _robot->getPosition();
    if(destination.y > 0)
    {
       if(_robot->getOrientarion().x < 0 )
           Global::communication->writeMessage(_robot->getPosMessage(), 40 + 129, 40);
       else
            Global::communication->writeMessage(_robot->getPosMessage(), 40, 40 + 129);
    }
    else
    {
        if(_robot->getOrientarion().x < 0 )
            Global::communication->writeMessage(_robot->getPosMessage(), 40, 40 + 129);
        else
            Global::communication->writeMessage(_robot->getPosMessage(), 40 + 129, 40);
    }
}

std::string GoalkeeperStateTurnaround::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(Global::eAreaDeffend == AREA_DEFFEND_LEFT)
    {
        if(Global::areaToDeffend.x + Global::areaToDeffend.width < _robot->getPosition().x ||
            Global::areaToDeffend.y > _robot->getPosition().y ||
            Global::areaToDeffend.y + Global::areaToDeffend.height < _robot->getPosition().y)
            return "retreating";

        if(_robot->getPosition().x <= Global::areaToDeffend.x)
            return "retreating";
    }

    if(Global::eAreaDeffend == AREA_DEFFEND_RIGHT)
    {
        if (Global::areaToDeffend.x > _robot->getPosition().x || Global::areaToDeffend.y > _robot->getPosition().y ||
            Global::areaToDeffend.y + Global::areaToDeffend.height < _robot->getPosition().y)
            return "retreating";
        if(_robot->getPosition().x >= Global::areaToDeffend.x + Global::areaToDeffend.width)
            return "retreating";
    }


    Vector2D comp(1, 0);
    Vector2D destination = Global::ball - _robot->getPosition();

   // if(abs((int)destination.y) < 15)
       // return "align";

    if(((_robot->getOrientarion()&&comp) < -7 && destination.y > 0) || ((_robot->getOrientarion()&&comp) > 7 && destination.y < 0))
        return "seeking";

    return "";
}

void GoalkeeperStateTurnaround::entryActions()
{
    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}

void GoalkeeperStateTurnaround::exitActions()
{
    Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
}