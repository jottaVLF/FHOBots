#include "AttackerStateAttacking.hpp"
#include "../../Global.hpp"

AttackerStateAttacking::AttackerStateAttacking(Robot* robot) : State("attacking"), _robot(robot)
{}

AttackerStateAttacking::~AttackerStateAttacking()
{}

void AttackerStateAttacking::doActions()
{
    Vector2D destination;
    destination = Global::areaGoalAttack.getCenter();
    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string AttackerStateAttacking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(!WorldModel::isAlignedWith(_robot->getOrientation(), Global::ball - _robot->getPosition()))
        return "seeking";

    return "";
}

void AttackerStateAttacking::entryActions()
{
    _robot->setPD(115, 220);
    _robot->setBasePwmValue(190);
    Vector2D robotToDestiny = Global::ball - _robot->getPosition();
    _robot->setLastError((_robot->getOrientation()||robotToDestiny));
}

void AttackerStateAttacking::exitActions()
{}