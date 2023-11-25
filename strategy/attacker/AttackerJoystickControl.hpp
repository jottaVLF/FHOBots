#ifndef FHOBOTS_ATTACKERJOYSTICKCONTROL_HPP
#define FHOBOTS_ATTACKERJOYSTICKCONTROL_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class AttackerJoystickControl : public State{
 public:
    AttackerJoystickControl(Robot * robot);
    ~AttackerJoystickControl();
    void doActions();
    std::string checkConditions();
    void entryActions();
    void exitActions();

private:
    Robot * _robot;
};


#endif