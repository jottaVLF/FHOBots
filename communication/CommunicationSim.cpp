#include "CommunicationSim.hpp"
#include <iostream>

CommunicationSim::CommunicationSim(int clientId){
    this->_clientId = clientId;
    // Get all motors handlers
    simxGetObjectHandle(this->_clientId, "/Right_Motor_B0", &this->_r0RightJointHandler, simx_opmode_blocking);
    simxGetObjectHandle(this->_clientId, "/Left_Motor_B0", &this->_r0LeftJointHandler, simx_opmode_blocking); 

    simxGetObjectHandle(this->_clientId, "/Right_Motor_B1", &this->_r1RightJointHandler, simx_opmode_blocking);
    simxGetObjectHandle(this->_clientId, "/Left_Motor_B1", &this->_r1LeftJointHandler, simx_opmode_blocking); 

    simxGetObjectHandle(this->_clientId, "/Right_Motor_B2", &this->_r2RightJointHandler, simx_opmode_blocking);
    simxGetObjectHandle(this->_clientId, "/Left_Motor_B2", &this->_r2LeftJointHandler, simx_opmode_blocking);    

    this->_r0LeftJointVelocity = this->_r0RightJointVelocity = 0;
    this->_r1LeftJointVelocity = this->_r1RightJointVelocity = 0;
    this->_r2LeftJointVelocity = this->_r2RightJointVelocity = 0;
}

CommunicationSim::~CommunicationSim(){

}

void CommunicationSim::writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight){
    simxFloat velocityLeft  = -pwmLeft  / 255. * 10;
    simxFloat velocityRight = -pwmRight / 255. * 10;

    switch(index){
        case 0:
            this->_r0LeftJointVelocity  = velocityLeft;
            this->_r0RightJointVelocity = velocityRight;
            break;
        case 1:
            this->_r1LeftJointVelocity  = velocityLeft;
            this->_r1RightJointVelocity = velocityRight;
            //std::cout << "{" << (int) pwmLeft << " " << (int) pwmRight << "} {" << velocityLeft << " " << velocityRight << "}\n";
            break;
        case 2:
            this->_r2LeftJointVelocity  = velocityLeft;
            this->_r2RightJointVelocity = velocityRight;
            break;
    }
}

        
void CommunicationSim::sendMessage(){
    simxSetJointTargetVelocity(this->_clientId, this->_r0LeftJointHandler,  this->_r0LeftJointVelocity, simx_opmode_streaming);
    simxSetJointTargetVelocity(this->_clientId, this->_r0RightJointHandler, this->_r0RightJointVelocity, simx_opmode_streaming);

    simxSetJointTargetVelocity(this->_clientId, this->_r1LeftJointHandler,  this->_r1LeftJointVelocity, simx_opmode_streaming);
    simxSetJointTargetVelocity(this->_clientId, this->_r1RightJointHandler, this->_r1RightJointVelocity, simx_opmode_streaming);

    simxSetJointTargetVelocity(this->_clientId, this->_r2LeftJointHandler,  this->_r2LeftJointVelocity, simx_opmode_streaming);
    simxSetJointTargetVelocity(this->_clientId, this->_r2RightJointHandler, this->_r2RightJointVelocity, simx_opmode_streaming);
}

void CommunicationSim::stopAll(){
    simxSetJointTargetVelocity(this->_clientId, this->_r0LeftJointHandler, 0, simx_opmode_streaming);
    simxSetJointTargetVelocity(this->_clientId, this->_r0RightJointHandler, 0, simx_opmode_streaming);

    simxSetJointTargetVelocity(this->_clientId, this->_r1LeftJointHandler, 0, simx_opmode_streaming);
    simxSetJointTargetVelocity(this->_clientId, this->_r1RightJointHandler, 0, simx_opmode_streaming);

    simxSetJointTargetVelocity(this->_clientId, this->_r2LeftJointHandler, 0, simx_opmode_streaming);
    simxSetJointTargetVelocity(this->_clientId, this->_r2RightJointHandler, 0, simx_opmode_streaming);

}