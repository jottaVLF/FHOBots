#include "DefenderUnivector.hpp"
#include "../../Global.hpp"
#include "../../navigation/UnivectorField.hpp"
#include <cmath>
#include <vector>

namespace
{
    const double OBSTACLE_INFLUENCE  = 150.0;
    const double OBSTACLE_REPULSION  = 3.0;
    const double OBSTACLE_TANGENTIAL = 1.8;
    const double TARGET_ATTRACTION   = 1.0;
    const double LOOKAHEAD           = 50.0;
    const double OBSTACLE_RADIUS     = 28.0;
    const double PREDICTION_FRAMES   = 2.0;
    const double PREDICTION_MAX_PX   = 28.0;

    bool isFiniteVector(const Vector2D& v)
    {
        return std::isfinite(v.x) && std::isfinite(v.y);
    }

    Vector2D predictedPosition(Robot* robot)
    {
        const Vector2D pos = robot->getPosition();
        const Vector2D vel = robot->getVelocity();

        if(!isFiniteVector(pos) || !isFiniteVector(vel))
            return pos;

        double dx = vel.x * PREDICTION_FRAMES;
        double dy = vel.y * PREDICTION_FRAMES;
        const double disp = std::hypot(dx, dy);

        if(!std::isfinite(disp) || disp < 0.001)
            return pos;

        if(disp > PREDICTION_MAX_PX)
        {
            const double s = PREDICTION_MAX_PX / disp;
            dx *= s;
            dy *= s;
        }

        return Vector2D(pos.x + dx, pos.y + dy);
    }

    int clampPwm(int v)
    {
        if(v < 0)   return 0;
        if(v > 255) return 255;
        return v;
    }

    UnivectorField& defenderField()
    {
        static bool initialized = false;
        static UnivectorField field;
        if(!initialized)
        {
            UnivectorConfig c;
            c.obstacleInfluenceRadius = OBSTACLE_INFLUENCE;
            c.obstacleRepulsionGain   = OBSTACLE_REPULSION;
            c.obstacleTangentialGain  = OBSTACLE_TANGENTIAL;
            c.targetAttractionGain    = TARGET_ATTRACTION;
            c.lookAheadDistance       = LOOKAHEAD;
            field = UnivectorField(c);
            initialized = true;
        }
        return field;
    }

    std::vector<UnivectorObstacle> collectObstacles(const Robot* robot)
    {
        std::vector<UnivectorObstacle> obstacles;

        if(robot != &Global::attacker && isFiniteVector(Global::attacker.getPosition()))
            obstacles.push_back(UnivectorObstacle(Global::attacker.getPosition(), OBSTACLE_RADIUS));

        if(robot != &Global::goalkeeper && isFiniteVector(Global::goalkeeper.getPosition()))
            obstacles.push_back(UnivectorObstacle(Global::goalkeeper.getPosition(), OBSTACLE_RADIUS));

        for(std::size_t i = 0; i < Global::enemyTeam.size(); i++)
        {
            if(isFiniteVector(Global::enemyTeam[i]))
                obstacles.push_back(UnivectorObstacle(Global::enemyTeam[i], OBSTACLE_RADIUS));
        }

        return obstacles;
    }
}

bool DefenderUnivector::applyNavigation(Robot* robot, const Vector2D& target)
{
    if(robot == nullptr || Global::communication == nullptr)
        return false;

    if(Global::ballPos.x < 0.0)
        return false;

    if(!isFiniteVector(robot->getPosition()) ||
       !isFiniteVector(robot->getOrientation()) ||
       !isFiniteVector(target))
        return false;

    UnivectorInput input;
    input.robotPosition    = predictedPosition(robot);
    input.robotOrientation = robot->getOrientation();
    input.targetPosition   = target;
    input.ballPosition     = Global::ball;
    input.obstacles        = collectObstacles(robot);

    const UnivectorOutput output = defenderField().compute(input);

    if(!output.valid || !isFiniteVector(output.waypoint))
        return false;

    // Controle proporcional direto no angleError do campo vetorial.
    // Evita oscilação do termo derivativo (kd) e o problema base > maxPwm do PD.
    const int base = std::max(60, std::min(200, (int)robot->getBasePwmValue()));
    const int turn = (int)(output.angleError / M_PI * base * 1.6);

    if(output.reverse)
    {
        // Em ré: inverter diferencial e flags de direção
        Global::communication->writeMessage(
            robot->getPosMessage(),
            clampPwm(base - turn),
            clampPwm(base + turn),
            true, true);
        return true;
    }

    Global::communication->writeMessage(
        robot->getPosMessage(),
        clampPwm(base + turn),
        clampPwm(base - turn),
        false, false);
    return true;
}
