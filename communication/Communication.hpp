#ifndef FHOBOTS_COMMUNICATION_HPP
#define FHOBOTS_COMMUNICATION_HPP

#include <SerialPort.h>
#include <SerialStream.h>
#include <string>
#include "ICommunication.hpp"

class Communication : public ICommunication
{
    public:
        Communication(const std::string port);
        ~Communication();

        void writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight);
        void sendMessage();
        void stopAll();
        void configureRobots();
        std::string getMessage();

    private:
        LibSerial::SerialStream _serial;
        char _message[8];
};


#endif //FHOBOTS_COMMUNICATION_HPP
