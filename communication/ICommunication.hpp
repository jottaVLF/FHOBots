#ifndef FHOBOTS_ICOMMUNICATION_HPP
#define FHOBOTS_ICOMMUNICATION_HPP

class ICommunication{

    public:
        virtual void writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight) = 0;

        virtual void sendMessage() = 0;

        virtual void stopAll() = 0;

};



#endif