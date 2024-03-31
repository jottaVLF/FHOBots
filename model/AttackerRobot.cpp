#include "AttackerRobot.hpp"
#include "../strategy/attacker/AttackerStateAttacking.hpp"
#include "../strategy/attacker/AttackerStateSeeking.hpp"
#include "../strategy/attacker/AttackerStateWaiting.hpp"
#include "../strategy/basic/StateIdle.hpp"
#include "../strategy/basic/StateBackOff.hpp"
#include "../strategy/attacker/AttackerStateSpinning.hpp"
#include "../strategy/attacker/AttackerStateAlign.hpp"
#include "../strategy/attacker/AttackerJoystickControl.hpp"
#include "../strategy/basic/StateExitArea.hpp"
#include "../Global.hpp"

AttackerRobot::AttackerRobot(const double kp, const double kd, const double basePwmValue) : Robot(kp, kd, basePwmValue)
{
    _posMessage = 1;

}

void AttackerRobot::createMachineStates(){
    this->_machineState.addState(new StateIdle(this));
    this->_machineState.addState(new StateBackOff(this));
    this->_machineState.addState(new AttackerStateAttacking(this));
    this->_machineState.addState(new AttackerStateSeeking(this));
    this->_machineState.addState(new AttackerStateWaiting(this));
    this->_machineState.addState(new AttackerStateSpinning(this));
    this->_machineState.addState(new AttackerStateAlign(this));
    this->_machineState.addState(new StateExitArea(this));
    this->_machineState.addState(new AttackerJoystickControl(this));

    this->_machineState.setState("idle");
}

void AttackerRobot::setJoystickState(){
    this->_machineState.setState("joystick");
}

AttackerRobot::~AttackerRobot()
{}


std::string AttackerRobot::getMessage()
{
    return Robot::getMessage();
}