#ifndef FHOBOTS_UNIVECTORFIELD_HPP
#define FHOBOTS_UNIVECTORFIELD_HPP

#include "Vector2D.hpp"
#include "Robot.hpp"

class UnivectorField {
public:
    static Vector2D guidePoint(Robot * robot, Vector2D target, double lookAhead = 45.0);

private:
    static Vector2D attractive(Vector2D from, Vector2D target);
    static Vector2D repulsive(Vector2D from, Vector2D obstacle, double influence, double gain);
    static Vector2D wallRepulsion(Vector2D from);
};

#endif
