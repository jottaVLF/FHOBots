#ifndef FHOBOTS_ROBOT_HPP
#define FHOBOTS_ROBOT_HPP

#include <chrono>
#include "../model/Vector2D.hpp"
#include "../control/Control.hpp"
#include "../strategy/MachineState.hpp"

class Robot {

    public:

        Vector2D lastPos;
        Vector2D lastOri;
        Robot(const double kp, const double kd, const double basePwmValue);
        virtual ~Robot();

        void calculatePwm(Vector2D& destination);
        void calculatePwmR(Vector2D &destination);

        void setOrientationRobot(const double x, const double y);
        void setOrientationRobot(const Vector2D v);

        void setPosition(const double x, const double y);
        void setPosition(const Vector2D v);
        void setLastPosition(const double x, const double y);


        Vector2D& getPosition();
        Vector2D& getOrientarion();
        Vector2D& getLastPosition();

        void setBasePwmValue(const double pwm);
        void setPwmLeft(const int pwm);
        void setPwmRight(const int pwm);
        void setPD(const double kp, const double kd);
        void setLastError(const double lError);
        void setMaxPwm(const int maxPwm = 160);

        int getBasePwmValue();
        int getPwmLeft();
        int getPwmRight();

        int getPosMessage();
        virtual std::string getMessage();

        void updateRobot();

        int unchangedPosCounter;
        bool forceSeeking;

    protected:

        int _posMessage;
        MachineState _machineState;

        virtual void createMachineStates() = 0;

    private:

        Vector2D _orientation;
        Vector2D _position;
        Control _control;
        Vector2D _lastPosition;
};


#endif //FHOBOTS_ROBOT_HPP
