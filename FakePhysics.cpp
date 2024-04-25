#include "FakePhysics.hpp"

void Physics::updateRobots(){
    
    updateRobot(&Global::attacker);
    updateRobot(&Global::deffender);
    updateRobot(&Global::goalkeeper);

}

void Physics::updateRobot(Robot * robot){
    double dt = 0.1;
    double wl = 47. / 255. * robot->getPwmLeft();
    double wr = 47. / 255. * robot->getPwmRight();

    wl = robot->reverseLeft ? -wl : wl;
    wr = robot->reverseRight? -wr : wr;

    double v = (wl + wr) * 0.35 / 2;
    double w = (wl - wr) * 0.35 / 0.7;

    double angle = atan2(robot->getOrientation().y, robot->getOrientation().x);
    angle = angle + w * dt;

    double x = robot->getPosition().x + v*cos(angle)*dt;
    double y = robot->getPosition().y + v*sin(angle)*dt;
    robot->setOrientationRobot(cos(angle),sin(angle));
    robot->setPosition(x, y);
    
}