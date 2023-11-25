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