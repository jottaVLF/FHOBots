#ifndef FHOBOTS_ICOMMUNICATION_HPP
#define FHOBOTS_ICOMMUNICATION_HPP

#include "../config/Config.hpp"

class ICommunication{

    public:
        virtual void writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight, const bool reverseLeft = false, const bool reverseRight = false) = 0;

        virtual void sendMessage() = 0;

        virtual void stopAll() = 0;

        virtual void configureRobots(Config config) = 0;

};



#endif