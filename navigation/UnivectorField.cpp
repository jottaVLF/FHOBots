#include "UnivectorField.hpp"
#include <algorithm>
#include <cmath>

namespace
{
    const double PI = 3.14159265358979323846;

    bool isFinite(double value)
    {
        return std::isfinite(value);
    }

    bool isFiniteVector(const Vector2D& value)
    {
        return isFinite(value.x) && isFinite(value.y);
    }

    double norm(double x, double y)
    {
        return std::hypot(x, y);
    }

    double normalizedAngle(double angle)
    {
        while(angle > PI)
            angle -= 2.0 * PI;

        while(angle < -PI)
            angle += 2.0 * PI;

        return angle;
    }
}

UnivectorObstacle::UnivectorObstacle()
    : position(0.0, 0.0), radius(0.0)
{}

UnivectorObstacle::UnivectorObstacle(const Vector2D& obstaclePosition, double obstacleRadius)
    : position(obstaclePosition), radius(obstacleRadius)
{}

UnivectorConfig::UnivectorConfig()
    : obstacleInfluenceRadius(90.0),
      obstacleRepulsionGain(1.2),
      obstacleTangentialGain(0.75),
      targetAttractionGain(1.0),
      lookAheadDistance(45.0),
      minVectorMagnitude(0.001),
      reverseHysteresis(0.2),
      reverseSwitchFrames(3)
{}

UnivectorOutput::UnivectorOutput()
    : valid(false),
      reverse(false),
      desiredDirection(0.0, 0.0),
      waypoint(0.0, 0.0),
      angleError(0.0),
      distanceToTarget(0.0)
{}

UnivectorField::UnivectorField(const UnivectorConfig& config)
    : _config(config),
      _hasReverseState(false),
      _lastReverse(false),
      _candidateReverse(false),
      _candidateReverseFrames(0)
{}

bool UnivectorField::chooseReverseMode(double frontError, double reverseError)
{
    const bool desiredReverse =
        std::fabs(reverseError) + _config.reverseHysteresis < std::fabs(frontError);

    if(!_hasReverseState)
    {
        _hasReverseState = true;
        _lastReverse = desiredReverse;
        _candidateReverse = desiredReverse;
        _candidateReverseFrames = 0;
        return _lastReverse;
    }

    if(desiredReverse == _lastReverse)
    {
        _candidateReverse = desiredReverse;
        _candidateReverseFrames = 0;
        return _lastReverse;
    }

    if(desiredReverse != _candidateReverse)
    {
        _candidateReverse = desiredReverse;
        _candidateReverseFrames = 1;
        return _lastReverse;
    }

    _candidateReverseFrames++;
    if(_candidateReverseFrames >= std::max(1, _config.reverseSwitchFrames))
    {
        _lastReverse = desiredReverse;
        _candidateReverseFrames = 0;
    }

    return _lastReverse;
}

UnivectorOutput UnivectorField::compute(const UnivectorInput& input)
{
    UnivectorOutput output;

    if(!isFiniteVector(input.robotPosition) ||
       !isFiniteVector(input.robotOrientation) ||
       !isFiniteVector(input.targetPosition))
        return output;

    const double targetX = input.targetPosition.x - input.robotPosition.x;
    const double targetY = input.targetPosition.y - input.robotPosition.y;
    const double targetDistance = norm(targetX, targetY);
    output.distanceToTarget = targetDistance;

    if(!isFinite(targetDistance) || targetDistance < _config.minVectorMagnitude)
        return output;

    const double targetUnitX = targetX / targetDistance;
    const double targetUnitY = targetY / targetDistance;

    double fieldX = _config.targetAttractionGain * targetUnitX;
    double fieldY = _config.targetAttractionGain * targetUnitY;

    for(std::size_t i = 0; i < input.obstacles.size(); i++)
    {
        const UnivectorObstacle& obstacle = input.obstacles[i];

        if(!isFiniteVector(obstacle.position) || obstacle.radius < 0.0)
            continue;

        const double awayX = input.robotPosition.x - obstacle.position.x;
        const double awayY = input.robotPosition.y - obstacle.position.y;
        const double obstacleX = obstacle.position.x - input.robotPosition.x;
        const double obstacleY = obstacle.position.y - input.robotPosition.y;
        const double centerDistance = norm(awayX, awayY);
        const double clearance = centerDistance - obstacle.radius;
        const double obstacleAhead = obstacleX * targetUnitX + obstacleY * targetUnitY;

        if(!isFinite(centerDistance) || centerDistance < _config.minVectorMagnitude)
            continue;

        if(clearance >= _config.obstacleInfluenceRadius)
            continue;

        const double normalizedClearance =
            std::max(0.0, (_config.obstacleInfluenceRadius - clearance) / _config.obstacleInfluenceRadius);
        const double strength = _config.obstacleRepulsionGain * normalizedClearance * normalizedClearance;

        double repulsionX = awayX / centerDistance;
        double repulsionY = awayY / centerDistance;

        if(obstacleAhead > 0.0)
        {
            const double backwardComponent = repulsionX * targetUnitX + repulsionY * targetUnitY;
            if(backwardComponent < 0.0)
            {
                repulsionX -= backwardComponent * targetUnitX;
                repulsionY -= backwardComponent * targetUnitY;
            }
        }

        fieldX += strength * repulsionX;
        fieldY += strength * repulsionY;

        const double lateralDistance = std::fabs(targetUnitX * obstacleY - targetUnitY * obstacleX);
        const double corridorRadius = obstacle.radius + 25.0;

        if(obstacleAhead > 0.0 && lateralDistance < corridorRadius)
        {
            double side = (targetUnitX * obstacleY - targetUnitY * obstacleX) >= 0.0 ? -1.0 : 1.0;
            if(lateralDistance < _config.minVectorMagnitude)
                side = 1.0;

            const double corridorWeight = 1.0 - (lateralDistance / corridorRadius);
            const double tangentialStrength = _config.obstacleTangentialGain * strength * corridorWeight;
            fieldX += tangentialStrength * (-targetUnitY) * side;
            fieldY += tangentialStrength * targetUnitX * side;
        }
    }

    const double fieldMagnitude = norm(fieldX, fieldY);
    if(!isFinite(fieldMagnitude) || fieldMagnitude < _config.minVectorMagnitude)
        return output;

    output.desiredDirection.set(fieldX / fieldMagnitude, fieldY / fieldMagnitude);

    const double waypointDistance = std::min(_config.lookAheadDistance, targetDistance);
    output.waypoint.set(input.robotPosition.x + output.desiredDirection.x * waypointDistance,
                        input.robotPosition.y + output.desiredDirection.y * waypointDistance);

    if(!isFiniteVector(output.waypoint))
        return output;

    double orientationX = input.robotOrientation.x;
    double orientationY = input.robotOrientation.y;
    double orientationMagnitude = norm(orientationX, orientationY);
    if(!isFinite(orientationMagnitude) || orientationMagnitude < _config.minVectorMagnitude)
    {
        orientationX = 1.0;
        orientationY = 0.0;
        orientationMagnitude = 1.0;
    }

    orientationX /= orientationMagnitude;
    orientationY /= orientationMagnitude;

    const double desiredAngle = std::atan2(output.desiredDirection.y, output.desiredDirection.x);
    const double frontAngle = std::atan2(orientationY, orientationX);
    const double reverseAngle = std::atan2(-orientationY, -orientationX);
    const double frontError = normalizedAngle(desiredAngle - frontAngle);
    const double reverseError = normalizedAngle(desiredAngle - reverseAngle);

    output.reverse = chooseReverseMode(frontError, reverseError);
    output.angleError = output.reverse ? reverseError : frontError;
    output.valid = true;

    return output;
}
