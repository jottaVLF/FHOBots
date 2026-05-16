#ifndef COMMUNICATION_WIFI
#define COMMUNICATION_WIFI

#include "ICommunication.hpp"
#include <string>
#include <iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

class CommunicationWIFI : public ICommunication
{
        public:
            CommunicationWIFI(std::string ip, int port);

            void writeMessage(const int index, const int pwmLeft, const int pwmRight, const bool reverseLeft = false, const bool reverseRight = false);

            void sendMessage();

            void stopAll();

            void getMessage();

            int getLeftPwm(int id);
            int getRightPwm(int id);
        
        private:
            sockaddr_in _server;
            unsigned char _writeBuffer[255];
            int _sockFd; 
};




#endif
