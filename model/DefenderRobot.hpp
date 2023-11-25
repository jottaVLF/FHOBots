#ifndef FHOBOTS_DEFENDERROBOT_HPP
#define FHOBOTS_DEFENDERROBOT_HPP

#include "Robot.hpp"

class DefenderRobot : public Robot
{
    public:
        DefenderRobot(const double kp, const double kd, const double basePwmValue);
        ~DefenderRobot();

        std::string getMessage();

        void createMachineStates();
};


#endif //FHOBOTS_DEFENDERROBOT_HPP
