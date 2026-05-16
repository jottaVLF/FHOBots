#ifndef FHOBOTS_ICOMMUNICATION_HPP
#define FHOBOTS_ICOMMUNICATION_HPP

class ICommunication{

    public:
        virtual void writeMessage(const int index, const int pwmLeft, const int pwmRight, const bool reverseLeft = false, const bool reverseRight = false) = 0;

        virtual void sendMessage() = 0;

        virtual void stopAll() = 0;

        virtual void getMessage() = 0;

        virtual int getLeftPwm(int id) = 0;
        virtual int getRightPwm(int id) = 0;
};



#endif
