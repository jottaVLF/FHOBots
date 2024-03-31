#include "Global.hpp"
#include "math.h"

int Global::pwmRightAtt(int pwm)
{
    return pwm;
}

int Global::pwmRightDef(int pwm)
{
    return pwm;
}

int Global::pwmComp1(int x)
{
    return ((-7.79211*pow(10, -11))*pow(x, 6)) + (0.000000076*pow(x, 5)) - ((2.9755852*(pow(10, -5))) * pow(x, 4)) + (0.0060084848*pow(x, 3))- (0.666876*(pow(x, 2))) + (40.1719*x) - 963.7713;
}

int Global::pwmComp2(int x) {
    return (4.36803741*(pow(10, -9))*(pow(x, 5))) - ((3.133948481375*(pow(10, -6))) * pow(x, 4)) + (0.0008532601*pow(x, 3)) - (0.110152335*pow(x, 2)) + (7.2416313272*x) - 140.818908535;
}

int Global::pwmGBComp(int x) {
    return (1.55972074167721*pow(10 , -7)*pow(x,5))-(6.03292891467719*pow(10,-5)*pow(x,4))+(0.009072017977046*pow(x,3))-(0.666765837289242*pow(x,2))+(24.9783858707265*x)-322.200118713726;
}

int Global::pwmGFComp(int x) {
    return (5.0678733032*pow(10,-7)*pow(x,5))-(0.0001908679556*pow(x,4))+(0.0271007815714*pow(x,3))-(1.8224434389143*pow(x,2))+(59.686855889225*x)-711.1925136928;
}

int Global::pwmGoalB(int x) {
    return (1.0417*x +11.03035);
}

int Global::pwmGoalF(int x) {
    return (2.12499*x - 33.99);
}

int Global::pwmRightGol(int pwm)
{
    return pwm;
}

int Global::pwmRightGolR(int pwm)
{
    return pwm;
}

bool Global::robotNearBall(Vector2D& posRobot, const int dist)
{
    return (ball - posRobot).magnitude() < dist * frameCentimetersConstant;
}

bool Global::robotNearBall2(Vector2D& posRobot, const int dist)
{
    return (ballPos - posRobot).magnitude() < dist * frameCentimetersConstant;
}

bool Global::robotFarFromBall(Vector2D& posRobot, const int dist)
{
    return (ball - posRobot).magnitude() > dist * frameCentimetersConstant;
}

bool Global::isInsideOwnArea(Vector2D& pos)
{
    return (pos.x >= areaToDeffend.x - areaToDeffend.width/2  && pos.x <= areaToDeffend.x + areaToDeffend.width/2 &&
            pos.y >= areaToDeffend.y - areaToDeffend.height/2 && pos.y <= areaToDeffend.y + areaToDeffend.height/2);
}

bool Global::isInsideOwnGoal(Vector2D &pos) {
    return (pos.x >= areaGoalDeffend.x && pos.x <= areaGoalDeffend.x + areaGoalDeffend.width &&
            pos.y >= areaGoalDeffend.y && pos.y <= areaGoalDeffend.y + areaGoalDeffend.height);
}

bool Global::robotNearRobot(Robot *robot, const int dist)
{
    if(robot != &attacker && (robot->getPosition() - Global::attacker.getPosition()).magnitude() < dist * frameCentimetersConstant && abs(robot->getOrientation()||Global::attacker.getOrientation()) > 1.55)
        return true;

    if(robot != &deffender && (robot->getPosition() - Global::deffender.getPosition()).magnitude() < dist * frameCentimetersConstant && abs(robot->getOrientation()||Global::deffender.getOrientation()) > 1.55)
        return true;

    if(robot != &goalkeeper && (robot->getPosition() - Global::goalkeeper.getPosition()).magnitude() < dist * frameCentimetersConstant && abs(robot->getOrientation()||Global::goalkeeper.getOrientation()) > 1.55)
        return true;

    return false;
}

bool Global::robotInOwnSide(Vector2D &posRobot)
{
    if(eAreaDeffend == AREA_DEFFEND_LEFT && posRobot.x <= fieldRect.width/2)
        return true;
    if(eAreaDeffend == AREA_DEFFEND_RIGHT && posRobot.x >= fieldRect.width/2)
        return true;

    return false;
}

bool Global::robotNearEnemyRobot(Vector2D &posRobot) {
    for(int i=0; i<Global::enemyTeam.size(); i++)
    {
        if((posRobot - enemyTeam[i]).magnitude() <= 8 *frameCentimetersConstant)
            return true;
    }
    return false;
}


Vector2D Global::ballPos;
Vector2D Global::lastBallPos;
Vector2D Global::ballVel;
Vector2D Global::ball;
Vector2D Global::centerGoalAttack;

ICommunication * Global::communication;

AttackerRobot Global::attacker(40.5, 0, 100.);
DefenderRobot Global::deffender(40.5, 0, 100.);
GoalkeeperRobot Global::goalkeeper(-5, 2.5, 60);

char Global::bufferKeyboard{'0'};

Area Global::areaGoalAttack;
Area Global::areaGoalDeffend;
Area Global::areaToAttack;
Area Global::areaToDeffend;
Area Global::fieldRect;

double Global::frameCentimetersConstant = 0;
int Global::countFrameAttacker;
int Global::countFrameDefender;
int Global::ballInAreaCounter = 0;
bool Global::hasJoystick = false;

std::vector<Vector2D> Global::enemyTeam;

Vector2D Global::posPointAttack(0,0);
Vector2D Global::posPointDeffend(0,0);
Vector2D Global::posPointGoalkeep(0,0);
Vector2D Global::joystickDestination(0,0);

PositionArea Global::eAreaAttack;
PositionArea Global::eAreaDeffend;

Vector2D Global::centerAreaToAttack;
Vector2D Global::centerAreaToDeffend;