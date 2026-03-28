#include "GoalkeeperStateKicking.hpp"


GoalkeeperStateKicking::GoalkeeperStateKicking(Robot *robot) : State("kicking"), _robot(robot)
{}

GoalkeeperStateKicking::~GoalkeeperStateKicking()
{}

void GoalkeeperStateKicking::doActions()
{
    Vector2D destination = Global::ball;
    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(),  _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string GoalkeeperStateKicking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(!WorldModel::isBallNearDeffenceArea())
        return "return";
    
    return "";
}

void GoalkeeperStateKicking::entryActions()
{
    if(Global::isSim){
        // Calibrações para o simulador
        _robot->setBasePwmValue(95);
        _robot->setPD(40., 80.);
    }else{
        _robot->setBasePwmValue(95);
        _robot->setPD(40., 80.);
    }
}

void GoalkeeperStateKicking::exitActions()
{
}