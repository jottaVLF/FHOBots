#include "WorldModel.hpp"

int WorldModel::_offset = 50;
int WorldModel::_precision = 26;

bool WorldModel::isInsideDeffenseArea(Vector2D position){
    return Global::areaToDeffend.isInside(position);
}    

bool WorldModel::isInsideAttackArea(Vector2D position){
    return Global::areaToAttack.isInside(position);
}
bool WorldModel::isDeffenseFieldOnLeft(){
    double fieldCenter = Global::fieldRect.width/2;
    bool isAreaOnLeft = Global::areaToDeffend.isOnLeft(fieldCenter);
    return isAreaOnLeft;
}
bool WorldModel::isOnDeffenseField(Vector2D position){
    double fieldCenter = Global::fieldRect.width/2;
    bool isAreaOnLeft = Global::areaToDeffend.isOnLeft(fieldCenter);

    if(isAreaOnLeft && position.x < fieldCenter)
        return true;
    if(!isAreaOnLeft && position.x > fieldCenter)
        return true;
    return false;
}
    
bool WorldModel::isOnAttackField(Vector2D position){
    double fieldCenter = Global::fieldRect.width/2;
    bool isAreaOnLeft = Global::areaToAttack.isOnLeft(fieldCenter);

    if(isAreaOnLeft && position.x < fieldCenter)
        return true;
    if(!isAreaOnLeft && position.x > fieldCenter)
        return true;
    return false;
}

bool WorldModel::isAlignedWith(Vector2D v, Vector2D w){
    double angle = v.angleBetween(w);
    return fabs(angle) <= M_PI/6;
}

bool WorldModel::isNearWall(Vector2D v){
    int offset = WorldModel::_offset;
    double fieldWidth = Global::fieldRect.width;
    double fieldHeight = Global::fieldRect.height;

    double xmin =  Global::areaGoalDeffend.isOnLeft(fieldWidth/2) ? 
                   Global::areaGoalDeffend.width + offset
                  :Global::areaGoalAttack.width + offset;
    double xmax = Global::areaGoalDeffend.isOnLeft(fieldWidth/2) ? 
                   fieldWidth - (Global::areaGoalDeffend.width + offset)
                  :fieldWidth - (Global::areaGoalAttack.width + offset);
    
    double ymin = offset;
    double ymax = fieldHeight - offset;

    bool isInSafeZone = v.x >= xmin && v.x <= xmax && v.y >= ymin && v.y <= ymax;

    return !isInSafeZone;
}

bool WorldModel::isAlignedWithWall(Vector2D p, Vector2D o){
    int offset = WorldModel::_offset;
    double fieldWidth = Global::fieldRect.width;
    double fieldHeight = Global::fieldRect.height;

    double xmin =  Global::areaGoalDeffend.isOnLeft(fieldWidth/2) ? 
                   Global::areaGoalDeffend.width + offset
                  :Global::areaGoalAttack.width + offset;
    double xmax = Global::areaGoalDeffend.isOnLeft(fieldWidth/2) ? 
                   fieldWidth - (Global::areaGoalDeffend.width + offset)
                  :fieldWidth - (Global::areaGoalAttack.width + offset);
    
    double ymin = offset;
    double ymax = fieldHeight - offset;

    Vector2D wallVector;

    if(p.x < xmin && p.y < ymin)
        wallVector.set(-1, -1);
    else if(p.x < xmin && p.y > ymax)
        wallVector.set(-1, 1);
    else if(p.x > xmax && p.y < ymin)
        wallVector.set(1, -1);
    else if(p.x > xmax && p.y > ymax)
        wallVector.set(1, 1);
    else if(p.x < xmin)
        wallVector.set(-1, 0);
    else if(p.x > xmax)
        wallVector.set(1, 0);
    else if(p.y < ymin)
        wallVector.set(0, -1);
    else if(p.y > ymax)
        wallVector.set(0, 1);
    else
        return false;

    return WorldModel::isAlignedWith(o, wallVector);
}

Robot * WorldModel::nearstRobotTo(Vector2D v){
    double distanceAttacker = WorldModel::distanceBetween(Global::attacker.getPosition(), v);
    double distanceDeffender = WorldModel::distanceBetween(Global::deffender.getPosition(), v);
    double distanceGoalkeeper = WorldModel::distanceBetween(Global::goalkeeper.getPosition(), v);

    if(distanceAttacker < distanceDeffender && distanceAttacker < distanceGoalkeeper)
        return &Global::attacker;
    if(distanceDeffender < distanceAttacker && distanceDeffender < distanceGoalkeeper)
        return &Global::deffender;
    
    return &Global::goalkeeper;
}

double WorldModel::distanceBetween(Vector2D v, Vector2D w){
    return sqrt(pow(v.x- w.x, 2) + pow(v.y - w.y, 2));
}

bool WorldModel::isInAttackArea(Robot * r){
    return Global::areaToAttack.isInside(r->getPosition());
}

bool WorldModel::isInDeffenseArea(Robot * r){
    return Global::areaToDeffend.isInside(r->getPosition());
}

bool WorldModel::isInAttackArea(Vector2D v){
    return Global::areaToAttack.isInside(v);
}

bool WorldModel::isInDeffenseArea(Vector2D v){
    return Global::areaToDeffend.isInside(v);
}

void WorldModel::halfTime(){
    std::cout << "HalfTime " << std::endl;
    Area areaGoalAttack; 
    Area areaToAttack;

    Global::areaGoalAttack.copy(areaGoalAttack);
    Global::areaToAttack.copy(areaToAttack);

    Global::areaGoalAttack.set(Global::areaGoalDeffend);
    Global::areaToAttack.set(Global::areaToDeffend);

    Global::areaGoalDeffend.set(areaGoalAttack);
    Global::areaToDeffend.set(areaToAttack);

}

bool WorldModel::isFacing(Vector2D o, direction d){
    double angle = atan2(o.y, o.x);

    switch(d){
        case DOWN: return angle <= 2 * M_PI / 3 && angle >= M_PI / 3; 
        case UP:   return angle >= -2 * M_PI / 3 && angle <= -M_PI / 3; 
        case LEFT: return angle >= 2 * M_PI/3 || angle <= - 2 * M_PI / 3; 
        case RIGHT:return angle >= -M_PI / 3 && angle <= M_PI / 3; 
    }
    return false;
}

bool WorldModel::isFacingArea(Vector2D o, Area a){
    double fieldWidth = Global::fieldRect.width;
    direction d = a.isOnLeft(fieldWidth/2) ? LEFT : RIGHT;

    return WorldModel::isFacing(o, d);
}

bool WorldModel::otherRobotInDeffenseArea(Robot * r){
    bool isOtherInArea = false;
    if(r != &Global::attacker)
        isOtherInArea = isOtherInArea || WorldModel::isInDeffenseArea(&Global::attacker);
    if(r != &Global::deffender)
        isOtherInArea = isOtherInArea || WorldModel::isInDeffenseArea(&Global::deffender);
    if(r != &Global::goalkeeper)
        isOtherInArea = isOtherInArea || WorldModel::isInDeffenseArea(&Global::goalkeeper);
    
    return isOtherInArea;
}

bool WorldModel::isNearOf(Vector2D o, Vector2D p){
    Vector2D r= o-p;
    return r.magnitude() < WorldModel::_precision;
}

bool WorldModel::isAlignedWithWallAndBall(Vector2D robotPosition, Vector2D robotOrientation){
    Vector2D robotToBall = Global::ball - robotPosition;
    return isAlignedWithWall(robotPosition, robotOrientation) && isNearOf(robotPosition, Global::ball) && isAlignedWith(robotOrientation, robotToBall);

}

Vector2D WorldModel::getGoalKeeperDeffencePosition(){
    double upperGoalLimit = Global::areaGoalDeffend.y;
    double lowerGoalLimit = upperGoalLimit + Global::areaGoalDeffend.height;
    Vector2D destination = Global::ball;
    if(Global::ball.y > lowerGoalLimit)
        destination.set(Global::areaToDeffend.getCenter().x, lowerGoalLimit);
    else if(Global::ball.y < upperGoalLimit)
        destination.set(Global::areaToDeffend.getCenter().x, upperGoalLimit);
    else
        destination.set(Global::areaToDeffend.getCenter().x, Global::ball.y);
    return destination;
}

Vector2D WorldModel::getDeffenderDeffencePosition(){
    double fieldCenter = Global::fieldRect.width/2;
    bool isAreaOnLeft  = Global::areaToAttack.isOnLeft(fieldCenter);

    double x = isAreaOnLeft ? 3*Global::fieldRect.width/4 : Global::fieldRect.width/4;
    double y = Global::ball.y;
    Vector2D destination(x, y);
    return destination;
}

bool WorldModel::isInFrontOf(Vector2D v, Vector2D w){
    return fabs(v.angleBetween(w)) < M_PI / 2;
}

bool WorldModel::isStuckAtDeffenseGoal(Vector2D robotPosition, Vector2D robotOrientation){
    bool isInsideGoal = Global::areaGoalDeffend.isInside(robotPosition);
    
    if(!isInsideGoal)
        return false;
    
    Vector2D robotToGoal = Global::areaGoalDeffend.getCenter() - robotPosition;

    return !WorldModel::isAlignedWith(robotOrientation, robotToGoal);
}

bool WorldModel::isBallNearDeffenceArea(){
    double fieldCenter = Global::fieldRect.width/2;
    bool isAreaOnLeft  = Global::areaToDeffend.isOnLeft(fieldCenter);

    int xmin = isAreaOnLeft ? Global::areaToDeffend.x + Global::areaToDeffend.width
                            : 3 * Global::fieldRect.width/4;
    int xmax = isAreaOnLeft ? Global::fieldRect.width/4
                            : Global::areaToDeffend.x;

    return Global::ball.x > xmin && Global::ball.x < xmax;
}
