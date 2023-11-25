#include "AttackerStateSeeking.hpp"
#include "../../Global.hpp"
#include <iostream>
#include <iomanip>

AttackerStateSeeking::AttackerStateSeeking(Robot* robot) : State("seeking"), _robot(robot)
{}

AttackerStateSeeking::~AttackerStateSeeking()
{}

void AttackerStateSeeking::doActions()
{
    //Vector2D destination = _robot->lastPos - _robot->getPosition();
    Vector2D destination = Global::ball;
    if(destination.x >0 && destination.y > 0){
        std::cout << "On Seeking : "  << destination.x << " " << destination.y << std::endl;
        std::cout << "On Seeking : "  << this->_robot->getPosition().x << " " << this->_robot->getPosition().y << std::endl;
        std::cout << "Orientation: " << atan2(this->_robot->getOrientarion().y, this->_robot->getOrientarion().x) * 180. / M_PI << std::endl;
    }else
        return;
  /*  switch (Global::eAreaAttack)
    {
        case AREA_ATTACK_RIGHT:
            destination -= _robot->getPosition();
            // std::cout << "Vector to objective: " << destination.x << " " << destination.y << std::endl;
            if(destination.x <= 10)
                destination.set(Global::ball.x - 20, destination.y <= 0 ? Global::ball.y + 40 : Global::ball.y - 40);

            else if(destination.x <= 90 && (destination.y > 15 || destination.y < -15))
                destination.set(Global::ball.x, Global::ball.y);

            else if(destination.x > 110)
                destination.set(Global::ball.x - destination.x / 2, Global::ball.y);
            else
                destination = Global::ball;
            break;

        case AREA_ATTACK_LEFT:
          //  std::cout << destination.x << " - " << _robot->getPosition().x << " " << destination.y << " - " << _robot->getPosition().y;
            destination -= _robot->getPosition();
          //  std::cout << " = " << destination.x << " " << destination.y << std::endl;
          //  std::cout << "Vector to objective: " << destination.x << " " << destination.y << std::endl;
            if(destination.x >= -5)
                destination.set(Global::ball.x + 20, destination.y <= 0 ? Global::ball.y + 40 : Global::ball.y - 40);

            else if(destination.x >= -90 && (destination.y > 15 || destination.y < -15))
                destination.set(Global::ball.x, Global::ball.y);

            else if(destination.x < -110)
                destination.set(Global::ball.x - destination.x / 2,  Global::ball.y);
            else 
                destination = Global::ball;
           // break;
           break;
    }*/

    Global::posPointAttack = destination;
    _robot->calculatePwm(destination);
    _robot->setPwmRight(Global::pwmRightAtt(_robot->getPwmRight()));
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string AttackerStateSeeking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

 /*   if(Global::robotNearRobot(_robot))
        return "backoff";

    Vector2D posRobot = _robot->getPosition();
    Vector2D oriRrobot = _robot->getOrientarion(); /// Orientação do Robô
    Vector2D comp;

    comp.set(1.0, 0.0);
    if(posRobot.y - 3.75 * Global::frameCentimetersConstant < Global::frameCentimetersConstant * 10 && (oriRrobot&&comp) > 0)
        return "backoff";

    comp.set(-1.0, 0.0);
    if(posRobot.y + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.height - Global::frameCentimetersConstant * 10
       && (oriRrobot&&comp) > 0)
        return "backoff";

    comp.set(0.0, -1.0);
    if(posRobot.x - 3.75 * Global::frameCentimetersConstant < Global::frameCentimetersConstant * 20 && (oriRrobot&&comp) > 0)
        return "backoff";

    comp.set(0.0, 1.0);
    if(posRobot.x + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.width - Global::frameCentimetersConstant * 20
       && (oriRrobot&&comp) > 0)
        return "backoff";

    if((Global::attacker.getPosition() - Global::ball).magnitude() > (Global::deffender.getPosition() - Global::ball).magnitude())
        return "waiting";

   /* if(Global::eAreaAttack == AREA_ATTACK_LEFT && Global::ball.x > (Global::fieldRect.width / 2 + 20))
        return "waiting";

    if(Global::eAreaAttack == AREA_ATTACK_RIGHT && Global::ball.x < (Global::fieldRect.width / 2 - 20))
        return "waiting";

    if(Global::robotNearBall2(_robot->getPosition(), 9) && Global::eAreaAttack == AREA_ATTACK_RIGHT && _robot->getOrientarion().x > 0)
        return "attacking";

    if(Global::robotNearBall2(_robot->getPosition(), 9) && Global::eAreaAttack == AREA_ATTACK_LEFT && _robot->getOrientarion().x < 0)
        return "attacking";

    Vector2D robotToDestiny = Global::ball - _robot->getPosition();
    if(!Global::robotFarFromBall(_robot->getPosition()) && ((_robot->getOrientarion()||robotToDestiny) >= 0.4 || (_robot->getOrientarion()||robotToDestiny) <= -0.4))
        return "align";
    */
    return ""; 
}

void AttackerStateSeeking::entryActions()
{
    _robot->setPD(40, -10);
    _robot->setBasePwmValue(80);
    _robot->setMaxPwm(120);
    _robot->lastPos = _robot->getPosition();
    _robot->lastOri = _robot->getOrientarion();
}

void AttackerStateSeeking::exitActions()
{}