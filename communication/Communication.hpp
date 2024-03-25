#ifndef FHOBOTS_COMMUNICATION_HPP
#define FHOBOTS_COMMUNICATION_HPP

#include <SerialPort.h>
#include <SerialStream.h>
#include <string>
#include "ICommunication.hpp"
#include "../config/Config.hpp"

class Communication : public ICommunication
{
    public:
        Communication(const std::string port);
        ~Communication();

        void writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight);
        void sendMessage();
        void stopAll();
        void configureRobots(HardwareConfig hardwareConfig);
        std::string getMessage();

    private:
        LibSerial::SerialStream _serial;
        char _message[10];
};


#endif //FHOBOTS_COMMUNICATION_HPP
