#ifndef WORLD_MODEL_HPP
#define WORLD_MODEL_HPP

#include "Vector2D.hpp"
#include "../Global.hpp"
#include <cmath>
#include <algorithm>

class WorldModel{

public:
    static bool isInsideDeffenseArea(Vector2D position);    
    static bool isInsideAttackArea(Vector2D position);
    static bool isOnDeffenseField(Vector2D position);
    static bool isOnAttackField(Vector2D position);
    static bool isAlignedWith(Vector2D v, Vector2D w);
    static bool isNearWall(Vector2D v, int offset = 30);
    static bool isAlignedWithWall(Vector2D p, Vector2D o, int offset = 30);
    static Robot * nearstRobotTo(Vector2D v);

private:
    static double distanceBetween(Vector2D v, Vector2D w);
};

#endif