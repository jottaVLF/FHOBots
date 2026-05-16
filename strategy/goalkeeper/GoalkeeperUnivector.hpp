#ifndef FHOBOTS_GOALKEEPERUNIVECTOR_HPP
#define FHOBOTS_GOALKEEPERUNIVECTOR_HPP

#include "../../model/Robot.hpp"
#include "../../model/Vector2D.hpp"

namespace GoalkeeperUnivector
{
    bool applyNavigation(Robot* robot, const Vector2D& target);
}

#endif
