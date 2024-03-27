#ifndef FHOBOTS_COMMUNICATION_HPP
#define FHOBOTS_COMMUNICATION_HPP

#include <SerialPort.h>
#include <SerialStream.h>
#include <string>
#include <SerialPortConstants.h>
#include <sstream>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <cstring>
#include <unistd.h>
#include "ICommunication.hpp"
#include "../config/Config.hpp"

class Communication : public ICommunication
{
    public:
        Communication(const std::string port);
        ~Communication();

        void writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight, const bool reverseLeft = false, const bool reverseRight = false);
        void sendMessage();
        void stopAll();
        void configureRobots(Config config);
        std::string getMessage();

    private:
        LibSerial::SerialStream _serial;
        char _message[16];
        bool allRobotsConfigured(std::unordered_map<std::string, std::pair<HardwareConfig *, bool>> map);
        void printRobotsConfigured(std::unordered_map<std::string, std::pair<HardwareConfig *, bool>> map);
        void sendConfigurationToRobot(std::string xbee, HardwareConfig * configuration);
};


#endif //FHOBOTS_COMMUNICATION_HPP
