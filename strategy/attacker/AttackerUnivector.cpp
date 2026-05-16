#include "AttackerUnivector.hpp"
#include "../../Global.hpp"
#include "../../navigation/UnivectorField.hpp"
#include <algorithm>
#include <cmath>
#include <deque>
#include <vector>

namespace
{
    const bool ENABLE_ATTACKER_UNIVECTOR = true;
    const double ATTACKER_UNIVECTOR_OBSTACLE_INFLUENCE = 135.0;
    const double ATTACKER_UNIVECTOR_OBSTACLE_REPULSION = 2.2;
    const double ATTACKER_UNIVECTOR_OBSTACLE_TANGENTIAL = 1.30;
    const double ATTACKER_UNIVECTOR_TARGET_ATTRACTION = 1.0;
    const double ATTACKER_UNIVECTOR_LOOKAHEAD = 50.0;
    const double ATTACKER_UNIVECTOR_OBSTACLE_RADIUS = 24.0;
    const double DEFENSE_AREA_OBSTACLE_RADIUS = 35.0;
    const double DEFENSE_AREA_OBSTACLE_MARGIN = 18.0;
    const double BALL_APPROACH_OFFSET_MIN = 24.0;
    const double BALL_APPROACH_OFFSET_MAX = 58.0;
    const double BALL_APPROACH_NEAR_DISTANCE = 35.0;
    const double BALL_APPROACH_FAR_DISTANCE = 145.0;
    const double CLOSE_BALL_DISTANCE = 70.0;
    const double CLOSE_BALL_DIRECT_WEIGHT = 0.65;
    const double CLOSE_BALL_OBSTACLE_RADIUS_SCALE = 0.45;
    const double LATERAL_ALIGNMENT_START = 0.45;
    const double LATERAL_ALIGNMENT_FULL = 0.85;
    const double LATERAL_ALIGNMENT_DIRECT_WEIGHT = 0.55;
    const double ROBOT_POSITION_PREDICTION_FRAMES = 2.0;
    const double ROBOT_POSITION_PREDICTION_MAX_PIXELS = 28.0;
    const std::size_t ENEMY_SMOOTHING_FRAMES = 3;
    const int SEEKING_LEFT_PWM_TRIM = -10;

    double clamp01(double value)
    {
        if(value < 0.0)
            return 0.0;

        if(value > 1.0)
            return 1.0;

        return value;
    }

    double interpolate(double start, double end, double amount)
    {
        return start + (end - start) * clamp01(amount);
    }

    double distanceBetween(const Vector2D& a, const Vector2D& b)
    {
        return std::hypot(a.x - b.x, a.y - b.y);
    }

    bool isFiniteVector(const Vector2D& value)
    {
        return std::isfinite(value.x) && std::isfinite(value.y);
    }

    Vector2D predictedRobotPosition(Robot* robot)
    {
        if(robot == nullptr)
            return Vector2D(0, 0);

        const Vector2D position = robot->getPosition();
        const Vector2D velocity = robot->getVelocity();

        if(!isFiniteVector(position) || !isFiniteVector(velocity))
            return position;

        double dx = velocity.x * ROBOT_POSITION_PREDICTION_FRAMES;
        double dy = velocity.y * ROBOT_POSITION_PREDICTION_FRAMES;
        const double displacement = std::hypot(dx, dy);

        if(!std::isfinite(displacement) || displacement < 0.001)
            return position;

        if(displacement > ROBOT_POSITION_PREDICTION_MAX_PIXELS)
        {
            const double scale = ROBOT_POSITION_PREDICTION_MAX_PIXELS / displacement;
            dx *= scale;
            dy *= scale;
        }

        return Vector2D(position.x + dx, position.y + dy);
    }

    int clampPwm(int pwm)
    {
        if(pwm < 0)
            return 0;

        if(pwm > 255)
            return 255;

        return pwm;
    }

    void addObstacle(std::vector<UnivectorObstacle>& obstacles, const Vector2D& position, double radius, double radiusScale)
    {
        if(!isFiniteVector(position))
            return;

        obstacles.push_back(UnivectorObstacle(position, std::max(1.0, radius * radiusScale)));
    }

    void addObstacle(std::vector<UnivectorObstacle>& obstacles, const Vector2D& position, double radiusScale)
    {
        addObstacle(obstacles, position, ATTACKER_UNIVECTOR_OBSTACLE_RADIUS, radiusScale);
    }

    void addDefenseAreaObstacles(std::vector<UnivectorObstacle>& obstacles, double radiusScale)
    {
        if(Global::areaToDeffend.width <= 0 || Global::areaToDeffend.height <= 0)
            return;

        const double xMin = Global::areaToDeffend.x - DEFENSE_AREA_OBSTACLE_MARGIN;
        const double xMax = Global::areaToDeffend.x + Global::areaToDeffend.width + DEFENSE_AREA_OBSTACLE_MARGIN;
        const double yMin = Global::areaToDeffend.y - DEFENSE_AREA_OBSTACLE_MARGIN;
        const double yMax = Global::areaToDeffend.y + Global::areaToDeffend.height + DEFENSE_AREA_OBSTACLE_MARGIN;
        const int segments = 4;
        const double defenseRadiusScale = std::max(0.70, radiusScale);

        for(int i = 0; i <= segments; i++)
        {
            const double t = static_cast<double>(i) / static_cast<double>(segments);
            const double x = xMin + (xMax - xMin) * t;
            const double y = yMin + (yMax - yMin) * t;

            addObstacle(obstacles, Vector2D(x, yMin), DEFENSE_AREA_OBSTACLE_RADIUS, defenseRadiusScale);
            addObstacle(obstacles, Vector2D(x, yMax), DEFENSE_AREA_OBSTACLE_RADIUS, defenseRadiusScale);
            addObstacle(obstacles, Vector2D(xMin, y), DEFENSE_AREA_OBSTACLE_RADIUS, defenseRadiusScale);
            addObstacle(obstacles, Vector2D(xMax, y), DEFENSE_AREA_OBSTACLE_RADIUS, defenseRadiusScale);
        }
    }

    std::vector<Vector2D> smoothedEnemies()
    {
        static std::vector<std::deque<Vector2D> > history;
        const std::size_t enemyCount = Global::enemyTeam.size();

        if(history.size() != enemyCount)
            history.assign(enemyCount, std::deque<Vector2D>());

        std::vector<Vector2D> smoothed;
        smoothed.reserve(enemyCount);

        for(std::size_t i = 0; i < enemyCount; i++)
        {
            if(!isFiniteVector(Global::enemyTeam[i]))
                continue;

            history[i].push_back(Global::enemyTeam[i]);
            while(history[i].size() > ENEMY_SMOOTHING_FRAMES)
                history[i].pop_front();

            Vector2D average(0.0, 0.0);
            for(std::size_t j = 0; j < history[i].size(); j++)
            {
                average.x += history[i][j].x;
                average.y += history[i][j].y;
            }

            if(!history[i].empty())
            {
                average.x /= static_cast<double>(history[i].size());
                average.y /= static_cast<double>(history[i].size());
                smoothed.push_back(average);
            }
        }

        return smoothed;
    }

    double closeBallWeight(const Vector2D& robotPosition)
    {
        if(!isFiniteVector(robotPosition) || !isFiniteVector(Global::ball))
            return 0.0;

        const double distanceToBall = distanceBetween(robotPosition, Global::ball);
        if(!std::isfinite(distanceToBall) || distanceToBall >= CLOSE_BALL_DISTANCE)
            return 0.0;

        return (1.0 - distanceToBall / CLOSE_BALL_DISTANCE) * CLOSE_BALL_DIRECT_WEIGHT;
    }

    double obstacleRadiusScale(const Vector2D& robotPosition)
    {
        if(!isFiniteVector(robotPosition) || !isFiniteVector(Global::ball))
            return 1.0;

        const double distanceToBall = distanceBetween(robotPosition, Global::ball);
        if(!std::isfinite(distanceToBall) || distanceToBall >= CLOSE_BALL_DISTANCE)
            return 1.0;

        const double proximity = 1.0 - distanceToBall / CLOSE_BALL_DISTANCE;
        return interpolate(1.0, CLOSE_BALL_OBSTACLE_RADIUS_SCALE, proximity);
    }

    double lateralAlignmentWeight(const Vector2D& robotPosition)
    {
        const Vector2D ball = Global::ball;
        const Vector2D goal = Global::areaGoalAttack.getCenter();

        if(!isFiniteVector(robotPosition) || !isFiniteVector(ball) || !isFiniteVector(goal))
            return 0.0;

        const double robotToBallX = ball.x - robotPosition.x;
        const double robotToBallY = ball.y - robotPosition.y;
        const double ballToGoalX = goal.x - ball.x;
        const double ballToGoalY = goal.y - ball.y;
        const double robotToBallDistance = std::hypot(robotToBallX, robotToBallY);
        const double ballToGoalDistance = std::hypot(ballToGoalX, ballToGoalY);

        if(robotToBallDistance < 0.001 || ballToGoalDistance < 0.001)
            return 0.0;

        const double robotToBallUnitX = robotToBallX / robotToBallDistance;
        const double robotToBallUnitY = robotToBallY / robotToBallDistance;
        const double ballToGoalUnitX = ballToGoalX / ballToGoalDistance;
        const double ballToGoalUnitY = ballToGoalY / ballToGoalDistance;
        const double lateralAmount =
            std::fabs(robotToBallUnitX * ballToGoalUnitY - robotToBallUnitY * ballToGoalUnitX);

        const double amount =
            (lateralAmount - LATERAL_ALIGNMENT_START) /
            (LATERAL_ALIGNMENT_FULL - LATERAL_ALIGNMENT_START);

        return clamp01(amount) * LATERAL_ALIGNMENT_DIRECT_WEIGHT;
    }

    double combinedDirectWeight(const Vector2D& robotPosition)
    {
        const double closeWeight = closeBallWeight(robotPosition);
        const double lateralWeight = lateralAlignmentWeight(robotPosition);

        return clamp01(1.0 - (1.0 - closeWeight) * (1.0 - lateralWeight));
    }

    Vector2D waypointToward(const Vector2D& robotPosition, const Vector2D& target)
    {
        const double targetX = target.x - robotPosition.x;
        const double targetY = target.y - robotPosition.y;
        const double targetDistance = std::hypot(targetX, targetY);

        if(!std::isfinite(targetDistance) || targetDistance < 0.001)
            return target;

        const double distance = std::min(ATTACKER_UNIVECTOR_LOOKAHEAD, targetDistance);
        return Vector2D(robotPosition.x + (targetX / targetDistance) * distance,
                        robotPosition.y + (targetY / targetDistance) * distance);
    }

    Vector2D blendVector(const Vector2D& a, const Vector2D& b, double bWeight)
    {
        const double amount = clamp01(bWeight);
        return Vector2D(a.x * (1.0 - amount) + b.x * amount,
                        a.y * (1.0 - amount) + b.y * amount);
    }

    std::vector<UnivectorObstacle> collectObstacles(const Robot* robot, double radiusScale)
    {
        std::vector<UnivectorObstacle> obstacles;

        if(robot != &Global::deffender)
            addObstacle(obstacles, Global::deffender.getPosition(), radiusScale);

        if(robot != &Global::goalkeeper)
            addObstacle(obstacles, Global::goalkeeper.getPosition(), radiusScale);

        const std::vector<Vector2D> enemies = smoothedEnemies();
        for(std::size_t i = 0; i < enemies.size(); i++)
            addObstacle(obstacles, enemies[i], radiusScale);

        addDefenseAreaObstacles(obstacles, radiusScale);

        return obstacles;
    }

    UnivectorField createField()
    {
        UnivectorConfig config;
        config.obstacleInfluenceRadius = ATTACKER_UNIVECTOR_OBSTACLE_INFLUENCE;
        config.obstacleRepulsionGain = ATTACKER_UNIVECTOR_OBSTACLE_REPULSION;
        config.obstacleTangentialGain = ATTACKER_UNIVECTOR_OBSTACLE_TANGENTIAL;
        config.targetAttractionGain = ATTACKER_UNIVECTOR_TARGET_ATTRACTION;
        config.lookAheadDistance = ATTACKER_UNIVECTOR_LOOKAHEAD;
        return UnivectorField(config);
    }

    UnivectorField& attackerField()
    {
        static UnivectorField field = createField();
        return field;
    }

    Vector2D computeBallApproachTarget(const Vector2D& robotPosition)
    {
        const Vector2D ball = Global::ball;
        const Vector2D goal = Global::areaGoalAttack.getCenter();
        const double directionX = goal.x - ball.x;
        const double directionY = goal.y - ball.y;
        const double directionMagnitude = std::hypot(directionX, directionY);

        if(!isFiniteVector(ball) || !isFiniteVector(goal) || directionMagnitude < 0.001)
            return ball;

        const double distanceToBall = isFiniteVector(robotPosition) ?
            distanceBetween(robotPosition, ball) :
            BALL_APPROACH_NEAR_DISTANCE;
        const double offsetAmount =
            (distanceToBall - BALL_APPROACH_NEAR_DISTANCE) /
            (BALL_APPROACH_FAR_DISTANCE - BALL_APPROACH_NEAR_DISTANCE);
        const double offset =
            interpolate(BALL_APPROACH_OFFSET_MIN, BALL_APPROACH_OFFSET_MAX, offsetAmount);

        return Vector2D(ball.x - (directionX / directionMagnitude) * offset,
                        ball.y - (directionY / directionMagnitude) * offset);
    }
}

bool AttackerUnivector::isEnabled()
{
    return ENABLE_ATTACKER_UNIVECTOR;
}

Vector2D AttackerUnivector::ballApproachTarget(const Vector2D& robotPosition)
{
    return computeBallApproachTarget(robotPosition);
}

bool AttackerUnivector::applyNavigation(Robot* robot, const Vector2D& target, bool seekingTrim, bool approachBall)
{
    if(!ENABLE_ATTACKER_UNIVECTOR || robot == nullptr || Global::communication == nullptr)
        return false;

    if(Global::ballPos.x < 0.0)
        return false;

    if(!isFiniteVector(robot->getPosition()) ||
       !isFiniteVector(robot->getOrientation()) ||
       !isFiniteVector(target))
        return false;

    const Vector2D navigationPosition = predictedRobotPosition(robot);
    const Vector2D effectiveTarget = approachBall ? computeBallApproachTarget(navigationPosition) : target;
    if(!isFiniteVector(effectiveTarget))
        return false;

    UnivectorInput input;
    input.robotPosition = navigationPosition;
    input.robotOrientation = robot->getOrientation();
    input.targetPosition = effectiveTarget;
    input.ballPosition = Global::ball;
    input.obstacles = collectObstacles(robot, obstacleRadiusScale(navigationPosition));

    UnivectorOutput output = attackerField().compute(input);

    if(!output.valid || !isFiniteVector(output.waypoint))
        return false;

    Vector2D waypoint = output.waypoint;
    const double directWeight = combinedDirectWeight(navigationPosition);
    if(directWeight > 0.0)
    {
        const Vector2D alignmentTarget =
            blendVector(effectiveTarget,
                        computeBallApproachTarget(navigationPosition),
                        lateralAlignmentWeight(navigationPosition));
        const Vector2D directWaypoint = waypointToward(navigationPosition, alignmentTarget);
        waypoint = blendVector(waypoint, directWaypoint, directWeight);
    }

    if(!isFiniteVector(waypoint))
        return false;

    if(output.reverse)
    {
        robot->calculatePwmR(waypoint);
        Global::communication->writeMessage(robot->getPosMessage(),
                                            clampPwm(robot->getPwmRight()),
                                            clampPwm(robot->getPwmLeft()),
                                            true,
                                            true);
        return true;
    }

    robot->calculatePwm(waypoint);
    const int leftPwm = robot->getPwmLeft() + (seekingTrim ? SEEKING_LEFT_PWM_TRIM : 0);
    Global::communication->writeMessage(robot->getPosMessage(),
                                        clampPwm(leftPwm),
                                        clampPwm(robot->getPwmRight()),
                                        false,
                                        false);
    return true;
}
