#ifndef FHOBOTS_GLOBAL_HPP
#define FHOBOTS_GLOBAL_HPP

#include "opencv2/opencv.hpp"

#include "model/Vector2D.hpp"
#include "model/Area.hpp"
#include "model/AttackerRobot.hpp"
#include "model/DefenderRobot.hpp"
#include "model/GoalkeeperRobot.hpp"
#include "Coordinates.hpp"
#include "communication/Communication.hpp"
#include "control/Control.hpp"
#include <vector>
#include <cstdlib>

enum PositionArea
{
    AREA_NOT_SET,
    AREA_ATTACK_RIGHT,
    AREA_ATTACK_LEFT,
    AREA_DEFFEND_RIGHT,
    AREA_DEFFEND_LEFT
};

class Global {

    public:
        static int pwmRightAtt(int pwm);
        static int pwmRightDef(int pwm);
        static int pwmRightGol(int pwm);
        static int pwmRightGolR(int pwm);
        static int pwmComp1(int x);
        static int pwmComp2(int x);
        static int pwmGFComp(int x);
        static int pwmGBComp(int x);

        static int pwmGoalF(int x);
        static int pwmGoalB(int x);

        static bool robotNearBall(Vector2D& posRobot, const int dist = 6);
        static bool robotFarFromBall(Vector2D& posRobot, const int dist = 25);
        static bool isInsideOwnArea(Vector2D& pos);
        static bool robotNearRobot(Robot* robot, const int dist = 10);
        static bool robotNearBall2(Vector2D& posRobot, const int dist = 6);
        static bool robotInOwnSide(Vector2D& posRobot);
        static bool isInsideOwnGoal(Vector2D& pos);
        static bool robotNearEnemyRobot(Vector2D& posRobot);

        static Vector2D ballPos;
        static Vector2D lastBallPos;
        static Vector2D ballVel;
        static Vector2D ball; /// Vetor de Posição da bola
        static Vector2D centerGoalAttack; /// Centro do Gol de Ataque
        static AttackerRobot attacker;
        static DefenderRobot deffender;
        static GoalkeeperRobot goalkeeper;

        static std::vector<Vector2D> enemyTeam;

        static int countFrameAttacker;
        static int countFrameDefender;

        static char bufferKeyboard;

        static ICommunication * communication;

        static double frameCentimetersConstant;

        static int ballInAreaCounter;

        static Area areaGoalAttack;
        static Area areaGoalDeffend;
        static Area areaToAttack;
        static Area areaToDeffend;
        static Area fieldRect;

        static Vector2D posPointAttack;
        static Vector2D posPointDeffend;
        static Vector2D posPointGoalkeep;

        static PositionArea eAreaAttack;
        static PositionArea eAreaDeffend;
        static Vector2D centerAreaToAttack;
        static Vector2D centerAreaToDeffend;
        static bool isSim;
};


#endif //FHOBOTS_GLOBAL_HPP
