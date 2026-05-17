#include "UnivectorField.hpp"
#include "../Global.hpp"
#include <algorithm>

Vector2D UnivectorField::guidePoint(Robot * robot, Vector2D target, double lookAhead){
    Vector2D position = robot->getPosition();
    Vector2D direction = attractive(position, target);

    if(robot != &Global::attacker){
        Vector2D force = repulsive(position, Global::attacker.getPosition(), 70, 1.8);
        direction += force;
    }
    if(robot != &Global::deffender){
        Vector2D force = repulsive(position, Global::deffender.getPosition(), 70, 1.8);
        direction += force;
    }
    if(robot != &Global::goalkeeper){
        Vector2D force = repulsive(position, Global::goalkeeper.getPosition(), 70, 1.8);
        direction += force;
    }

    for(int i = 0; i < Global::enemyTeam.size(); i++){
        Vector2D force = repulsive(position, Global::enemyTeam[i], 65, 1.4);
        direction += force;
    }

    Vector2D force = wallRepulsion(position);
    direction += force;

    if(direction.magnitude() == 0)
        direction = attractive(position, target);

    direction.normalize();
    Vector2D step = direction * lookAhead;
    return position + step;
}

Vector2D UnivectorField::attractive(Vector2D from, Vector2D target){
    Vector2D direction = target - from;
    direction.normalize();
    return direction;
}

Vector2D UnivectorField::repulsive(Vector2D from, Vector2D obstacle, double influence, double gain){
    Vector2D away = from - obstacle;
    double distance = away.magnitude();
    if(distance == 0 || distance > influence)
        return Vector2D(0, 0);

    away.normalize();
    double strength = gain * (influence - distance) / influence;
    return away * strength;
}

Vector2D UnivectorField::wallRepulsion(Vector2D from){
    Vector2D force(0, 0);
    double margin = std::max(45.0, Global::frameCentimetersConstant * 18.0);
    double gain = 2.2;

    if(from.x < margin)
        force.x += gain * (margin - from.x) / margin;
    if(from.x > Global::fieldRect.width - margin)
        force.x -= gain * (from.x - (Global::fieldRect.width - margin)) / margin;
    if(from.y < margin)
        force.y += gain * (margin - from.y) / margin;
    if(from.y > Global::fieldRect.height - margin)
        force.y -= gain * (from.y - (Global::fieldRect.height - margin)) / margin;

    return force;
}
