#ifndef FHOBOTS_COMMUNICATIONSIM
#define FHOBOTS_COMMUNICATIONSIM

#include "../sim/extApi.h"
#include "ICommunication.hpp"

class CommunicationSim : public ICommunication{
    
    private:
        int _clientId;

        simxInt _r0LeftJointHandler;
        simxInt _r0RightJointHandler;

        simxInt _r1LeftJointHandler;
        simxInt _r1RightJointHandler;

        simxInt _r2LeftJointHandler;
        simxInt _r2RightJointHandler;

        simxFloat _r0LeftJointVelocity;
        simxFloat _r0RightJointVelocity;       

        simxFloat _r1LeftJointVelocity;
        simxFloat _r1RightJointVelocity;
    
        simxFloat _r2LeftJointVelocity;
        simxFloat _r2RightJointVelocity;

    public:
        CommunicationSim(int clientId);
        ~CommunicationSim();

        void writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight);

        void sendMessage();

        void stopAll();

        void configureRobots();
};
#endif