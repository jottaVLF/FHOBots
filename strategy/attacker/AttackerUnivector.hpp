#ifndef FHOBOTS_ATTACKERUNIVECTOR_HPP
#define FHOBOTS_ATTACKERUNIVECTOR_HPP

#include "../../model/Robot.hpp"
#include "../../model/Vector2D.hpp"

namespace AttackerUnivector
{
    bool isEnabled();
    Vector2D ballApproachTarget(const Vector2D& robotPosition);
    bool applyNavigation(Robot* robot, const Vector2D& target, bool seekingTrim, bool approachBall = false);
}

#endif
