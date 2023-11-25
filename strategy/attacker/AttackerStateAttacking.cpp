#include "AttackerStateAttacking.hpp"
#include "../../Global.hpp"

AttackerStateAttacking::AttackerStateAttacking(Robot* robot) : State("attacking"), _robot(robot)
{}

AttackerStateAttacking::~AttackerStateAttacking()
{}

void AttackerStateAttacking::doActions()
{
    Vector2D destination;
    destination = Global::centerGoalAttack;
    if(Global::ball.y > Global::centerGoalAttack.y + 10 && (Global::ball - destination).magnitude() <  25)
        destination.set(Global::areaGoalAttack.x + Global::areaGoalAttack.width, Global::areaGoalAttack.y);
    else if(Global::ball.y < Global::centerGoalAttack.y - 10 && (Global::ball - destination).magnitude() <  25 )
        destination.set(Global::areaGoalAttack.x + Global::areaGoalAttack.width, Global::areaGoalAttack.y + Global::areaGoalAttack.height);
    else
        destination = Global::centerGoalAttack;

    _robot->calculatePwm(destination);
    _robot->setPwmRight(Global::pwmRightAtt(_robot->getPwmRight()));
    Global::posPointAttack = destination;
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string AttackerStateAttacking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    if(Global::robotNearRobot(_robot))
        return "backoff";

    Vector2D posRobot = _robot->getPosition();
    Vector2D oriRrobot = _robot->getOrientarion(); /// Orientação do Robô
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

   /* if(Global::eAreaAttack == AREA_ATTACK_LEFT && Global::ball.x > (Global::fieldRect.width / 2 + 20))
        return "waiting";

    if(Global::eAreaAttack == AREA_ATTACK_RIGHT && Global::ball.x < (Global::fieldRect.width / 2 - 20))
        return "waiting";
*/
    if(!(Global::robotNearBall2(_robot->getPosition(), 12)))
        return "seeking";

    return "";
}

void AttackerStateAttacking::entryActions()
{
    _robot->setPD(115, 220);
    _robot->setBasePwmValue(190);
    Vector2D robotToDestiny = Global::ball - _robot->getPosition();
    _robot->setLastError((_robot->getOrientarion()||robotToDestiny));
}

void AttackerStateAttacking::exitActions()
{}