#ifndef FHOBOTS_ATTACKERROBOT_HPP
#define FHOBOTS_ATTACKERROBOT_HPP

#include "Robot.hpp"
#include <chrono>

class AttackerRobot : public Robot {

    public:
        AttackerRobot(const double kp, const double kd, const double basePwmValue);
        ~AttackerRobot();

        std::string getMessage();

        void createMachineStates();

};


#endif //FHOBOTS_ATTACKERROBOT_HPP
