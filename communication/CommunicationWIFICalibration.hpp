#ifndef COMMUNICATION_WIFICALIBRATION
#define COMMUNICATION_WIFICALIBRATION

#include <string>
#include <iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <netinet/in.h>


#include "jsonRead.hpp"
class CommunicationWIFICalibration
{
        public:
            CommunicationWIFICalibration(std::string ip, int port);

            void receiveMessage();

            void sendMessage();
            
        private:
            sockaddr_in _server;
            unsigned char _writeBuffer[255];
            int _sockFd; 
            jsonRead jsonHandler;
};




#endif