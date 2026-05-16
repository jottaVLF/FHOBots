#ifndef FAKE_PHYSICS_HPP
#define FAKE_PHYSICS_HPP
#include "model/Robot.hpp"
#include "Global.hpp"
#include <cmath>
class Physics{
public:
    static void updateRobots();
private:    
    static void updateRobot(Robot * robot);
};
#endif