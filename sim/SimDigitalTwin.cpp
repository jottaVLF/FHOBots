#include "SimDigitalTwin.hpp"
#include <iostream>

SimDigitalTwin::SimDigitalTwin(simxInt clientId, int xCamResolution, int yCamResolution){
    this->_clientId = clientId;
    this->_xCamResolution = xCamResolution;
    this->_yCamResolution = yCamResolution;
    simxInt code = simxGetObjectHandle(this->_clientId, "/Robot_B0", &this->_r0FhobotsHandle, simx_opmode_blocking);
    code = simxGetObjectHandle(this->_clientId, "/Robot_B1", &this->_r1FhobotsHandle, simx_opmode_blocking);
    code = simxGetObjectHandle(this->_clientId, "/Robot_B2", &this->_r2FhobotsHandle, simx_opmode_blocking);
}

void SimDigitalTwin::synchronize(){
    updateRobotOnSim(Global::attacker, this->_r0FhobotsHandle);
}


simxFloat SimDigitalTwin::toXSimCoord(int x){
    int xCamResolution = this->_xCamResolution;
    return (x - xCamResolution/2)*1.5/xCamResolution;
}

simxFloat SimDigitalTwin::toYSimCoord(int y){
    int yCamResolution = this->_yCamResolution;
    return -(y - yCamResolution/2)*1.3/yCamResolution;
}

void SimDigitalTwin::updateRobotOnSim(Robot & robot, simxInt handler){
    float pos[3];
    pos[0] = this->toXSimCoord(robot.getPosition().x);
    pos[1] = this->toYSimCoord(robot.getPosition().y);
    pos[2] = 0.0725;
    std::cout << pos[0] << "\t" << pos[1] << std::endl;
    simxSetObjectPosition(this->_clientId, handler, -1, pos, simx_opmode_streaming);
}