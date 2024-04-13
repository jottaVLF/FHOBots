#ifndef WORLD_MODEL_HPP
#define WORLD_MODEL_HPP

#include "Vector2D.hpp"
#include "../Global.hpp"
#include "Robot.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

enum direction{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class WorldModel{

public:
    static int _offset;
    static bool isInsideDeffenseArea(Vector2D position);    
    static bool isInsideAttackArea(Vector2D position);
    static bool isOnDeffenseField(Vector2D position);
    static bool isOnAttackField(Vector2D position);
    static bool isAlignedWith(Vector2D v, Vector2D w);
    static bool isNearWall(Vector2D v);
    static bool isAlignedWithWall(Vector2D p, Vector2D o);
    static Robot * nearstRobotTo(Vector2D v);
    static bool isInAttackArea(Robot * r);
    static bool isInDeffenseArea(Robot * r);
    static bool isInAttackArea(Vector2D v);
    static bool isInDeffenseArea(Vector2D v);
    static void halfTime();
    static bool isFacing(Vector2D o, direction d);
    static bool isFacingArea(Vector2D o, Area a);
    static bool otherRobotInDeffenseArea(Robot * r);
    static bool isNearOf(Vector2D o, Vector2D p);

private:
    static double distanceBetween(Vector2D v, Vector2D w);
};

#endif