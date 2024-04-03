#ifndef FHOBOTS_AREA_HPP
#define FHOBOTS_AREA_HPP

#include "Vector2D.hpp"
#include "opencv2/opencv.hpp"

class Area : public Vector2D{
    public:
        int width;
        int height;
        Area();
        Area(Vector2D center, int width, int height);
        Area(cv::Rect area);
        ~Area();
        bool isInside(Vector2D point);
        bool isOnLeft(double x);
        void copy(Area & other);
        void set(Area other);
};

#endif