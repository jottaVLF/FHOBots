#include "FakePhysics.hpp"

void Physics::updateRobots(){
    
    updateRobot(&Global::attacker);
    updateRobot(&Global::deffender);
    updateRobot(&Global::goalkeeper);

}

void Physics::updateRobot(Robot * robot){
    // Loop principal roda a ~60fps (cv::waitKey(16)).
    // dt = 0.1 causava saltos de ~90px/frame → sobrepasso → oscilação aparente.
    const double dt = 0.016;

    // FakeCommunication stores signed PWM (negative = reverse).
    // Reading from the communication layer is the only reliable source
    // because the UnivectorField path does not set robot->reverseLeft/Right.
    double wl, wr;
    if(Global::communication != nullptr)
    {
        const int id = robot->getPosMessage();
        wl = 47.0 / 255.0 * Global::communication->getLeftPwm(id);
        wr = 47.0 / 255.0 * Global::communication->getRightPwm(id);
    }
    else
    {
        wl = 47.0 / 255.0 * robot->getPwmLeft();
        wr = 47.0 / 255.0 * robot->getPwmRight();
        wl = robot->reverseLeft  ? -wl : wl;
        wr = robot->reverseRight ? -wr : wr;
    }

    // v in px/s: wheel angular velocity (rad/s) * radius (0.35dm) * scale (47px/dm)
    double v = (wl + wr) * 16.45 / 2;
    double w = (wl - wr) * 0.35 / 0.7;

    double angle = atan2(robot->getOrientation().y, robot->getOrientation().x);
    angle = angle + w * dt;

    double x = robot->getPosition().x + v*cos(angle)*dt;
    double y = robot->getPosition().y + v*sin(angle)*dt;
    robot->setOrientationRobot(cos(angle),sin(angle));
    robot->setPosition(x, y);
    
}