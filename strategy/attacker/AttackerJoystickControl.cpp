#include "AttackerJoystickControl.hpp"
#include "../../Global.hpp"

AttackerJoystickControl::AttackerJoystickControl(Robot *robot) : State("joystick"), _robot(robot) {

}

AttackerJoystickControl::~AttackerJoystickControl() {



}

void AttackerJoystickControl::doActions() {

    Vector2D destination = Global::joystickDestination;
  
    switch (Global::eAreaAttack)
    {
        case AREA_ATTACK_RIGHT:
            destination -= _robot->getPosition();
            if(destination.x <= 10)
                destination.set(Global::ball.x - 20, destination.y <= 0 ? Global::ball.y + 40 : Global::ball.y - 40);

            else if(destination.x <= 90 && (destination.y > 15 || destination.y < -15))
                destination.set(Global::ball.x, Global::ball.y);

            else if(destination.x > 110)
                destination.set(Global::ball.x - destination.x / 2, Global::ball.y);
            else
                destination = Global::ball;
            break;

        case AREA_ATTACK_LEFT:
         
           break;
    }

    Global::posPointAttack = destination;
    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string AttackerJoystickControl::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    return ""; 
}

void AttackerJoystickControl::entryActions()
{
    _robot->setPD(110, 200.85);
    _robot->setBasePwmValue(160);
    _robot->setMaxPwm(255);
    _robot->lastPos = _robot->getPosition();
    _robot->lastOri = _robot->getOrientation();
}

void AttackerJoystickControl::exitActions()
{}