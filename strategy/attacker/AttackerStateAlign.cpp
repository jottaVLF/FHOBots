#include "AttackerStateAlign.hpp"
#include "../../Global.hpp"
#include <iostream>

AttackerStateAlign::AttackerStateAlign(Robot *robot) : State("align"), _robot(robot)
{}

AttackerStateAlign::~AttackerStateAlign()
{}

void AttackerStateAlign::doActions()
{
    Vector2D destination = Global::ball;
    switch (Global::eAreaAttack)
    {
        case AREA_ATTACK_RIGHT:
            destination -= _robot->getPosition();
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
            destination -= _robot->getPosition();
            if(destination.x >= -5)
                destination.set(Global::ball.x + 20, destination.y <= 0 ? Global::ball.y + 40 : Global::ball.y - 40);

            else if(destination.x >= -90 && (destination.y > 15 || destination.y < -15))
                destination.set(Global::ball.x, Global::ball.y);

            else if(destination.x < -110)
                destination.set(Global::ball.x - destination.x / 2,  Global::ball.y);
            else
                destination = Global::ball;
            break;
    }

    _robot->calculatePwm(destination);
    _robot->setPwmRight(Global::pwmRightAtt(_robot->getPwmRight()));
    Global::posPointAttack = destination;
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string AttackerStateAlign::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(Global::robotNearRobot(_robot))
        return "backoff";

    Vector2D posRobot = _robot->getPosition();
    Vector2D oriRrobot = _robot->getOrientation(); /// Orientação do Robô
    Vector2D comp;

    comp.set(1.0, 0.0);
    if(posRobot.y - 3.75 * Global::frameCentimetersConstant < Global::frameCentimetersConstant * 10 && (oriRrobot&&comp) > 0.2)
        return "backoff";

    comp.set(-1.0, 0.0);
    if(posRobot.y + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.height - Global::frameCentimetersConstant * 10
       && (oriRrobot&&comp) > 0)
        return "backoff";

    comp.set(0.0, -1.0);
    if(posRobot.x - 3.75 * Global::frameCentimetersConstant < Global::frameCentimetersConstant * 20 && (oriRrobot&&comp) > 0.2)
        return "backoff";

    comp.set(0.0, 1.0);
    if(posRobot.x + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.width - Global::frameCentimetersConstant * 20
       && (oriRrobot&&comp) > 0.2)
        return "backoff";

    if(Global::robotNearBall2(_robot->getPosition(), 9))
        return "attacking";

    Vector2D robotToDestiny = Global::ball - _robot->getPosition();

    printf("%f\n", (_robot->getOrientation()||robotToDestiny));
    if(((_robot->getOrientation()||robotToDestiny) <= 0.25 && (_robot->getOrientation()||robotToDestiny) >= -0.25))
        return "seeking";

    if(Global::robotFarFromBall(_robot->getPosition()))
        return "seeking";

        return "";
}

void AttackerStateAlign::entryActions()
{
    _robot->setPD(100.55, 425);
    _robot->setMaxPwm(255);
    _robot->setBasePwmValue(100);
}

void AttackerStateAlign::exitActions()
{
    _robot->setMaxPwm(160);
}