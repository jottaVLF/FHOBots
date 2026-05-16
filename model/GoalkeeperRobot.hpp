#ifndef FHOBOTS_GOALKEEPERROBOT_HPP
#define FHOBOTS_GOALKEEPERROBOT_HPP

#include "Robot.hpp"

class GoalkeeperRobot : public  Robot
{
    public:
        GoalkeeperRobot(const double kp, const double kd, const double basePwmValue);
        ~GoalkeeperRobot();

        std::string getMessage();

        void createMachineStates();
};


#endif //FHOBOTS_GOALKEEPERROBOT_HPP
