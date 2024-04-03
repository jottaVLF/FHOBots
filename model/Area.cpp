#include "Area.hpp"

Area::Area(Vector2D center, int width, int height): Vector2D(center.x, center.y){
    this->height = height;
    this->width  = width;
}

Area::Area(cv::Rect area): Vector2D((double) area.x, (double) area.y){
    this->width  = area.width;
    this->height = area.height;
}
        
Area::Area(){
    
}

Area::~Area(){

}

bool Area::isInside(Vector2D point){
    double xp = point.x;
    double yp = point.y;
    double xmin = this->x - this->width/2;
    double xmax = this->x + this->width/2;
    double ymin = this->y - this->height/2;
    double ymax = this->y + this->height/2;

    return xp >= xmin && xp <= xmax && yp >= ymin && yp <= ymax;
}

bool Area::isOnLeft(double x){
    return this->x < x;
}

void Area::copy(Area & other){
    other.x = this->x;
    other.y = this->y;
    other.width = this->width;
    other.height = this->height;
}

void Area::set(Area other){
    this->x = other.x;
    this->y = other.y;
    this->width = other.width;
    this->height= other.height;
}