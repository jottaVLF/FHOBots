#include "JoystickSimulationInterface.hpp"
#include <iostream>
JoystickSimulationInterface::JoystickSimulationInterface(int clientId){
    simxFloat pos[3];
    this->_clientId = clientId;
    simxGetObjectHandle(clientId, "/Destination", &this->_destinationHandler, simx_opmode_blocking);   
    simxGetObjectPosition(this->_clientId, this->_destinationHandler, -1, pos, simx_opmode_streaming);
    Global::joystickDestination.set(this->toXPixelCoord(pos[0]), this->toYPixelCoord(pos[1]));
}

JoystickSimulationInterface::~JoystickSimulationInterface(){

}

void JoystickSimulationInterface::updateJoystickDestination(int inc_x, int inc_y){
    simxFloat pos[3];
    simxInt code = simxGetObjectPosition(this->_clientId, this->_destinationHandler, -1, pos, simx_opmode_buffer);
    pos[0] += inc_x * 0.1;
    pos[1] -= inc_y * 0.1;
    code = simxSetObjectPosition(this->_clientId, this->_destinationHandler, -1, pos, simx_opmode_oneshot);
    Global::joystickDestination.set(this->toXPixelCoord(pos[0]), this->toYPixelCoord(pos[1]));

}

int JoystickSimulationInterface::toXPixelCoord(simxFloat x){
    int xCamResolution = 640;
    return (int) (x/1.5*xCamResolution) + xCamResolution/2;
}

int JoystickSimulationInterface::toYPixelCoord(simxFloat y){
    int yCamResolution = 480;
    return -(int) (y/1.3*yCamResolution)+ yCamResolution/2;
}
