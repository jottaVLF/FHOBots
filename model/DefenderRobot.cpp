#include "DefenderRobot.hpp"
#include "../strategy/basic/StateIdle.hpp"
#include "../strategy/basic/StateBackOff.hpp"
#include "../strategy/defender/DefenderStateWaiting.hpp"
#include "../strategy/defender/DefenderStateSeeking.hpp"
#include "../strategy/defender/DefenderStateKicking.hpp"
#include "../strategy/defender/DefenderStateAlign.hpp"
#include "../strategy/attacker/AttackerStateSpinning.hpp"
#include "../strategy/basic/StateExitArea.hpp"

DefenderRobot::DefenderRobot(const double kp, const double kd, const double basePwmValue) : Robot(kp, kd, basePwmValue)
{
    _posMessage = 2;
    
}

void DefenderRobot::createMachineStates(){
    this->_machineState.addState(new StateIdle(this));
    this->_machineState.addState(new StateBackOff(this));
    this->_machineState.addState(new DefenderStateSeeking(this));
    this->_machineState.addState(new DefenderStateWaiting(this));
    this->_machineState.addState(new DefenderStateKicking(this));
    this->_machineState.addState(new AttackerStateSpinning(this));
    this->_machineState.addState(new DefenderStateAlign(this));
    this->_machineState.addState(new StateExitArea(this));
    this->_machineState.setState("idle");
}

DefenderRobot::~DefenderRobot()
{}

std::string DefenderRobot::getMessage()
{
    return Robot::getMessage();
}