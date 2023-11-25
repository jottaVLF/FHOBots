#ifndef FHOBOTS_JOYSTICKSIMULATIONINTERFACE_HPP
#define FHOBOTS_JOYSTICKSIMULATIONINTERFACE_HPP

#include "../sim/extApi.h"
#include "../Global.hpp"

class JoystickSimulationInterface{
private:
    int _clientId;
    simxInt _destinationHandler;
    int toXPixelCoord(simxFloat x);
    int toYPixelCoord(simxFloat y);
    
public:
    JoystickSimulationInterface(int clientId);
    ~JoystickSimulationInterface();
    void updateJoystickDestination(int inc_x, int inc_y);

};






#endif