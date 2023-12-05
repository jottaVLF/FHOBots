#include "VisionSim.hpp"
#include <iostream>
VisionSim::VisionSim(int clientId, int xCamResolution, int yCamResolution, bool debug){
    this->_clientId = clientId;
    this->_xCamResolution = xCamResolution;
    this->_yCamResolution = yCamResolution;
    this->debug = debug;
    simxInt code = simxGetObjectHandle(this->_clientId, "/Ball", &this->_ballHandle, simx_opmode_blocking);
    code = simxGetObjectHandle(this->_clientId, "/Robot_B0", &this->_r0FhobotsHandle, simx_opmode_blocking);
    code = simxGetObjectHandle(this->_clientId, "/Robot_B1", &this->_r1FhobotsHandle, simx_opmode_blocking);
    code = simxGetObjectHandle(this->_clientId, "/Robot_B2", &this->_r2FhobotsHandle, simx_opmode_blocking);
    simxFloat pos[3];
    simxGetObjectPosition(this->_clientId, this->_ballHandle, -1, pos, simx_opmode_streaming);
    simxGetObjectPosition(this->_clientId, this->_r0FhobotsHandle, -1, pos, simx_opmode_streaming);
    simxGetObjectPosition(this->_clientId, this->_r1FhobotsHandle, -1, pos, simx_opmode_streaming);
    simxGetObjectPosition(this->_clientId, this->_r2FhobotsHandle, -1, pos, simx_opmode_streaming);
    simxFloat angles[3];
    simxGetObjectOrientation(this->_clientId, this->_r0FhobotsHandle, -1, angles, simx_opmode_streaming);
    simxGetObjectOrientation(this->_clientId, this->_r1FhobotsHandle, -1, angles, simx_opmode_streaming);
    simxGetObjectOrientation(this->_clientId, this->_r2FhobotsHandle, -1, angles, simx_opmode_streaming);
}

void VisionSim::detectionColors(){
    simxFloat pos[3];
    simxGetObjectPosition(this->_clientId, this->_ballHandle, -1, pos, simx_opmode_buffer);

    Global::ball.set(this->toXPixelCoord(pos[0]), this->toYPixelCoord(pos[1]));
  //  std::cout << "On Vision : "  << Global::ball.x << " " << Global::ball.y << std::endl;
    this->updateRobotPosition(Global::goalkeeper,  this->_r0FhobotsHandle);
    this->updateRobotPosition(Global::attacker, this->_r1FhobotsHandle);
    this->updateRobotPosition(Global::deffender,this->_r2FhobotsHandle);
    
    this->updateRobotOrientation(Global::goalkeeper,  this->_r0FhobotsHandle);
    this->updateRobotOrientation(Global::attacker, this->_r1FhobotsHandle);
    this->updateRobotOrientation(Global::deffender,this->_r2FhobotsHandle);

    if(!this->debug)
        return;

    std::cout << "Attacker: "  << Global::attacker.getPosition().x << " " << Global::attacker.getPosition().y << std::endl;
    std::cout << "Deffender: " << Global::deffender.getPosition().x << " " << Global::deffender.getPosition().y << std::endl;
    std::cout << "GoalKeeper: " << Global::goalkeeper.getPosition().x << " " << Global::goalkeeper.getPosition().y << std::endl << std::endl;
}

void VisionSim::updateRobotPosition(Robot & r, int handle){
    simxFloat pos[3];
    simxGetObjectPosition(this->_clientId, handle, -1, pos, simx_opmode_buffer);
    r.setLastPosition(r.getPosition().x, r.getPosition().y);
    r.setPosition(this->toXPixelCoord(pos[0]), this->toYPixelCoord(pos[1]));
}

void VisionSim::updateRobotOrientation(Robot & r, int handle){
    simxFloat angles[3];
    simxGetObjectOrientation(this->_clientId, handle, -1, angles, simx_opmode_buffer);
    float angle = this->simAngleToMath(angles);
   /* std::cout << "(";
    for(int i = 0; i < 3; i++)
        std::cout << angles[i] << " ";    
    std::cout <<" ) " << angle << std::endl; */
    r.setOrientationRobot(cos(angle), -sin(angle));
}

VisionSim::~VisionSim(){
    
}

int VisionSim::toXPixelCoord(simxFloat x){
    int xCamResolution = this->_xCamResolution;
    return (int) (x/1.5*xCamResolution) + xCamResolution/2;  
}

int VisionSim::toYPixelCoord(simxFloat y){
    int yCamResolution = this->_yCamResolution;
    return -(int) (y/1.3*yCamResolution)+ yCamResolution/2;
}

void VisionSim::adjustFieldPosition(){
    
    this->getPositionForObject(Global::areaGoalDeffend,0.675, 0);
    this->getPositionForObject(Global::areaGoalAttack,-0.675, 0);
    this->getPositionForObject(Global::areaToAttack, -0.8, 0);
    
    this->getWidthForObject(Global::areaGoalDeffend, 0.6, 0.75);
    this->getHeightForObject(Global::areaGoalDeffend, -0.35, 0.35);
    
    this->getWidthForObject(Global::areaGoalAttack, -0.75, -0.6);
    this->getHeightForObject(Global::areaGoalAttack, -0.35, 0.35);
}

void VisionSim::calibration(){

}

void VisionSim::getPositionForObject(Vector2D & area, float centerX, float centerY){
    area.x = this->toXPixelCoord(centerX);
    area.y = this->toYPixelCoord(centerY);
}

void VisionSim::getHeightForObject(Area & area, float minY, float maxY){
    area.height = (maxY - minY) / 1.3 * this->_yCamResolution;
}

void VisionSim::getWidthForObject(Area & area, float minX, float maxX){
    area.width = (maxX - minX) / 1.5 * this->_xCamResolution;
}

bool VisionSim::isEqualAngle(float angle, float angleRef){
    return angle + 0.2 >= angleRef && angle - 0.2 <= angleRef;
}

float VisionSim::simAngleToMath(simxFloat * angles){

    if (angles[0] < 0 && angles[2] < 0)
            return M_PI_2 - angles[1];


    if (angles[0] > 0 && angles[2] > 0)
        return 3*M_PI_2 + angles[1];

    return 0;
}