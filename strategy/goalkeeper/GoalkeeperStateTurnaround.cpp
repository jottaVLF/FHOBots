#include "GoalkeeperStateTurnaround.hpp"
#include "../../Global.hpp"
#include <iostream>

GoalkeeperStateTurnaround::GoalkeeperStateTurnaround(Robot *robot) : State("turnaround"), _robot(robot)
{}

GoalkeeperStateTurnaround::~GoalkeeperStateTurnaround()
{}

void GoalkeeperStateTurnaround::doActions()
{
    
}

std::string GoalkeeperStateTurnaround::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

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