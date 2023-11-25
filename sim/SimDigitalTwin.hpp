#ifndef FHOBOTS_SIMDIGITALTWIN_HPP
#define FHOBOTS_SIMDIGITALTWIN_HPP

#include "extApi.h"
#include "../model/Robot.hpp"
#include "../Global.hpp"

class SimDigitalTwin{
public:
    SimDigitalTwin(simxInt clientId, int xCamResolution, int yCamResolution);

    void synchronize();

private:
    simxInt _clientId;

    int _xCamResolution;
    int _yCamResolution;
    simxInt _r0FhobotsHandle;
    simxInt _r1FhobotsHandle;
    simxInt _r2FhobotsHandle;

    simxFloat toXSimCoord(int x);
    simxFloat toYSimCoord(int y);

    void updateRobotOnSim(Robot & robot, simxInt handler);
};






#endif