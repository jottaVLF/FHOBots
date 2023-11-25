#include "DefenderStateKicking.hpp"
#include "../../Global.hpp"

DefenderStateKicking::DefenderStateKicking(Robot* robot) : State("kicking"), _robot(robot)
{}

DefenderStateKicking::~DefenderStateKicking()
{}

void DefenderStateKicking::doActions()
{
    Vector2D destination;

    if(Global::ball.y > Global::centerGoalAttack.y + 4)
        destination.set(Global::areaGoalAttack.x + Global::areaGoalAttack.width, Global::areaGoalAttack.y);
    else if(Global::ball.y < Global::centerGoalAttack.y - 4)
        destination.set(Global::areaGoalAttack.x + Global::areaGoalAttack.width, Global::areaGoalAttack.y + Global::areaGoalAttack.height);
    else
        destination = Global::centerGoalAttack;

    _robot->calculatePwm(destination);
    _robot->setPwmRight(Global::pwmRightDef(_robot->getPwmRight()));
    Global::posPointDeffend = destination;
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string DefenderStateKicking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(Global::ball.x == -10)
        return "spinning";

    if(!(Global::robotNearBall(_robot->getPosition())))
        return "seeking";

    Vector2D posRobot = _robot->getPosition();
    Vector2D oriRobot = _robot->getOrientarion(); /// Orientação do Robô
    Vector2D comp;

    comp.set(1.0, 0.0);
    if(posRobot.y < Global::frameCentimetersConstant * 10 && (oriRobot&&comp) > 0.50)
        return "backoff";

    comp.set(-1.0, 0.0);
    if(posRobot.y + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.height - Global::frameCentimetersConstant * 10
       && (oriRobot&&comp) > 0.50)
        return "backoff";

    comp.set(0.0, -1.0);
    if(posRobot.x < Global::frameCentimetersConstant * 20 && (oriRobot&&comp) > 0.50)
        return "backoff";

    comp.set(0.0, 1.0);
    if(posRobot.x + 3.75 * Global::frameCentimetersConstant > Global::fieldRect.width - Global::frameCentimetersConstant * 20
       && (oriRobot&&comp) > 0.50)
        return "backoff";

    return "";
}

void DefenderStateKicking::entryActions()
{
    _robot->setPD(90, 50);
}

void DefenderStateKicking::exitActions()
{}