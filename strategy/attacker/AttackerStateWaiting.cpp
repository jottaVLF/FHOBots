#include "AttackerStateWaiting.hpp"
#include "../../Global.hpp"
#include "../../model/WorldModel.hpp"
#include "iostream"

AttackerStateWaiting::AttackerStateWaiting(Robot* robot) : State("waiting"), _robot(robot)
{}

AttackerStateWaiting::~AttackerStateWaiting()
{}

void AttackerStateWaiting::doActions()
{
    _robot->setPwmLeft(0);
    _robot->setPwmRight(0);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string AttackerStateWaiting::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(WorldModel::isInDeffenseArea(_robot))
        return "backoff";

    if(WorldModel::isAlignedWithWall(_robot->getPosition(), _robot->getOrientation()))
        return "backoff";

    if(WorldModel::isNearOf(Global::deffender.getPosition(), _robot->getPosition()))
        return "backoff";

    if(WorldModel::isOnAttackField(Global::ball) && WorldModel::isOnDeffenseField(Global::deffender.getPosition()))
        return "seeking";

    if(WorldModel::nearstRobotTo(Global::ball) == _robot && !WorldModel::isInDeffenseArea(_robot))
        return "seeking";

    return "";
}

void AttackerStateWaiting::entryActions()
{
        _robot->setPD(110, 200.85);
        _robot->setBasePwmValue(180);
        _robot->setMaxPwm(255);
    _robot->lastPos = _robot->getPosition();
    _robot->lastOri = _robot->getOrientation();
}

void AttackerStateWaiting::exitActions()
{
    Vector2D robotToDestiny = Global::ball - _robot->getPosition();
    _robot->setLastError((_robot->getOrientation()||robotToDestiny));
}