#ifndef FHOBOTS_DEFENDERUNIVECTOR_HPP
#define FHOBOTS_DEFENDERUNIVECTOR_HPP

#include "../../model/Robot.hpp"
#include "../../model/Vector2D.hpp"

namespace DefenderUnivector
{
    bool applyNavigation(Robot* robot, const Vector2D& target);
}

#endif
