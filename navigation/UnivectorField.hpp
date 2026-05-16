#ifndef FHOBOTS_UNIVECTORFIELD_HPP
#define FHOBOTS_UNIVECTORFIELD_HPP

#include "../model/Vector2D.hpp"
#include <vector>

struct UnivectorObstacle
{
    UnivectorObstacle();
    UnivectorObstacle(const Vector2D& obstaclePosition, double obstacleRadius);

    Vector2D position;
    double radius;
};

struct UnivectorConfig
{
    UnivectorConfig();

    double obstacleInfluenceRadius;
    double obstacleRepulsionGain;
    double obstacleTangentialGain;
    double targetAttractionGain;
    double lookAheadDistance;
    double minVectorMagnitude;
    double reverseHysteresis;
    int reverseSwitchFrames;
};

struct UnivectorInput
{
    Vector2D robotPosition;
    Vector2D robotOrientation;
    Vector2D targetPosition;
    Vector2D ballPosition;
    std::vector<UnivectorObstacle> obstacles;
};

struct UnivectorOutput
{
    UnivectorOutput();

    bool valid;
    bool reverse;
    Vector2D desiredDirection;
    Vector2D waypoint;
    double angleError;
    double distanceToTarget;
};

class UnivectorField
{
    public:
        explicit UnivectorField(const UnivectorConfig& config = UnivectorConfig());

        UnivectorOutput compute(const UnivectorInput& input);

    private:
        bool chooseReverseMode(double frontError, double reverseError);

        UnivectorConfig _config;
        bool _hasReverseState;
        bool _lastReverse;
        bool _candidateReverse;
        int _candidateReverseFrames;
};

#endif
