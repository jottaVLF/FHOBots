#include "GoalkeeperRobot.hpp"
#include "../strategy/basic/StateIdle.hpp"
#include "../strategy/goalkeeper/GoalkeeperStateSeeking.hpp"
#include "../strategy/goalkeeper/GoalkeeperStateRetreating.hpp"
#include "../strategy/goalkeeper/GoalkeeperStateKicking.hpp"
#include "../strategy/goalkeeper/GoalkeeperStateTurnaround.hpp"
#include "../strategy/goalkeeper/GoalkeeperStateAlign.hpp"
#include "../strategy/basic/StateBackOff.hpp"
#include "../strategy/goalkeeper/GoalkeeperStateMoveBack.hpp"
#include "../strategy/goalkeeper/GoalkeeperMoveForward.hpp"
#include "../strategy/goalkeeper/GoalkeeperStateSpinning.hpp"
#include "../strategy/goalkeeper/GoalkeeperStateWaiting.hpp"
#include "../strategy/goalkeeper/GoalkeeperStateReturnToArea.hpp"
#include "../strategy/goalkeeper/GoalkeeperStateExitGoal.hpp"

GoalkeeperRobot::GoalkeeperRobot(const double kp, const double kd, const double basePwmValue) : Robot(kp, kd, basePwmValue)
{
    _posMessage = 0;
    setMaxPwm(80);
}

void GoalkeeperRobot::createMachineStates(){
    this->_machineState.addState(new StateIdle(this));
    this->_machineState.addState(new GoalkeeperStateSeeking(this));
    this->_machineState.addState(new GoalkeeperStateKicking(this));
    this->_machineState.addState(new StateBackOff(this));
    this->_machineState.addState(new GoalkeeperStateMoveBack(this));
    this->_machineState.addState(new GoalkeeperMoveForward(this));
    this->_machineState.addState(new GoalkeeperStateSpinning(this));
    this->_machineState.addState(new GoalkeeperStateWaiting(this));
    this->_machineState.addState(new GoalkeeperStateReturnToArea(this));
    this->_machineState.addState(new GoalkeeperStateExitGoal(this));
    this->_machineState.setState("idle");
}

GoalkeeperRobot::~GoalkeeperRobot()
{}

std::string GoalkeeperRobot::getMessage()
{
    return "Goalkeeper, " + Robot::getMessage();
}