#include "GoalkeeperStateAlign.hpp"
#include <cmath>
#include "../../Global.hpp"

GoalkeeperStateAlign::GoalkeeperStateAlign(Robot *robot) : State("align"), _robot(robot)
{}

GoalkeeperStateAlign::~GoalkeeperStateAlign()
{}
void GoalkeeperStateAlign::alignWith(Vector2D objective){
    Vector2D robotToObjective = objective - _robot->getPosition();
    if(WorldModel::isDeffenseFieldOnLeft()){
        if(_robot->getOrientation().y > 0)
          _robot->spinClockWise(50);
        else
         _robot->spinCounterClockWise(50);
    }
    else{
        if(_robot->getOrientation().y > 0)
          _robot->spinCounterClockWise(50);
        else
         _robot->spinClockWise(50);
    }
    
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight(), _robot->reverseLeft, _robot->reverseRight);
}

void GoalkeeperStateAlign::doActions()
{
    

    if(WorldModel::isNearOf(_robot->getPosition(),Global::areaToDeffend.getCenter())){
        Vector2D yAxis(0,1);
        alignWith(yAxis);
        
    }
    
}

std::string GoalkeeperStateAlign::checkConditions()
{
    Vector2D yAxis(0,1);
    Vector2D yAxisToRobot = yAxis - _robot->getPosition();
    Vector2D robotToCenterArea = Global::areaToDeffend.getCenter() - _robot->getPosition();
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(fabs(_robot->getErrorAngleTo(yAxisToRobot)) < M_PI / 10 && WorldModel::isNearOf(_robot->getPosition(),Global::areaToDeffend.getCenter()))
        return "seeking";
    

    if(WorldModel::isInsideDeffenseArea(_robot->getPosition()) && !WorldModel::isNearOf(_robot->getPosition(),Global::areaGoalDeffend.getCenter()))
        return "seeking";
    return "";
}

void GoalkeeperStateAlign::entryActions()
{
    _robot->setPD(100.55, 425);
    _robot->setMaxPwm(255);
    _robot->setBasePwmValue(100);
}

void GoalkeeperStateAlign::exitActions()
{
    _robot->moveForward(0);
    _robot->setMaxPwm(160);
}