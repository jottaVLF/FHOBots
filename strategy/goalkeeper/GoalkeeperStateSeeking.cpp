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
    Vector2D destination;
    destination = Global::ball - _robot->getPosition();

    if(fabs(destination.y) < 1)
    {
        Global::communication->writeMessage(_robot->getPosMessage(), 0, 0);
        return;
    }

    destination.set(Global::eAreaDeffend == AREA_DEFFEND_LEFT ? Global::areaGoalDeffend.x + Global::areaGoalDeffend.width + 10 * Global::frameCentimetersConstant :
                    Global::areaGoalDeffend.x - 10 * Global::frameCentimetersConstant, Global::ball.y);

    if(Global::ball.y > Global::areaGoalDeffend.y + Global::areaGoalDeffend.height)
        destination.set(Global::eAreaDeffend == AREA_DEFFEND_LEFT ? Global::areaGoalDeffend.x + Global::areaGoalDeffend.width + 10 * Global::frameCentimetersConstant :
                        Global::areaGoalDeffend.x - 10 * Global::frameCentimetersConstant, Global::areaGoalDeffend.y + Global::areaGoalDeffend.height);

    if(Global::ball.y < Global::areaGoalDeffend.y)
        destination.set(Global::eAreaDeffend == AREA_DEFFEND_LEFT ? Global::areaGoalDeffend.x + Global::areaGoalDeffend.width + 10 * Global::frameCentimetersConstant :
                        Global::areaGoalDeffend.x - 10 * Global::frameCentimetersConstant, Global::areaGoalDeffend.y);
    _robot->calculatePwm(destination);
    
    Global::posPointGoalkeep = destination;
    Global::communication->writeMessage(_robot->getPosMessage(),  _robot->getPwmLeft(), Global::pwmGFComp(_robot->getPwmRight()));
}

std::string GoalkeeperStateSeeking::checkConditions()
{

    return "waiting";
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(Global::eAreaDeffend == AREA_DEFFEND_LEFT) {
        if (Global::areaToDeffend.x + Global::areaToDeffend.width < _robot->getPosition().x ||
            Global::areaToDeffend.y > _robot->getPosition().y ||
            Global::areaToDeffend.y + Global::areaToDeffend.height < _robot->getPosition().y ||
            Global::areaToDeffend.x + 8 > _robot->getPosition().x)
            return "retreating";
    }

    if(Global::eAreaDeffend == AREA_DEFFEND_RIGHT) {
        if (Global::areaToDeffend.x > _robot->getPosition().x || Global::areaToDeffend.y > _robot->getPosition().y ||
            Global::areaToDeffend.y + Global::areaToDeffend.height < _robot->getPosition().y ||
            Global::areaToDeffend.x + Global::areaToDeffend.width - 8 < _robot->getPosition().x)
            return "retreating";
    }

    if(Global::isInsideOwnArea(Global::ballPos) && Global::robotNearBall(_robot->getPosition(), 10))
        return "kicking";

    Vector2D comp;
    comp.set(1, 0);
    Vector2D destination = Global::ball - _robot->getPosition();
    if(((_robot->getOrientation()&&comp) > -8 && destination.y > 10) ||
       ((_robot->getOrientation()&&comp) < 8 && destination.y < -10))
        return "turnaround";

    if(((_robot->getPosition().y >= Global::areaGoalDeffend.y + Global::areaGoalDeffend.height &&
         _robot->getOrientation().y > 0) || (_robot->getPosition().y <= Global::areaGoalDeffend.y &&
                                             _robot->getOrientation().y < 0)))
        return "align";



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