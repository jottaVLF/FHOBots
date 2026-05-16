#ifndef FHOBOTS_FIELD_HPP
#define FHOBOTS_FIELD_HPP

#include "opencv2/opencv.hpp"
#include "../vision/Mouse.hpp"
#include "../model/Area.hpp"

class Field {

    public:
        Field();
        Field(const std::string name);

        ~Field();

        cv::Point getCenterPosition();
        cv::Rect getRect();

        int getWidth();
        int getHeight();

        void setRect(const Area rect);
        void draw(cv::Mat frame);
        void setWindow();

        bool isSelected();

        std::string getName();
        void setWindowName(std::string windowName);

    private:
        Mouse _mouse;
        std::string _name;

        cv::Rect toCvRect(Area area);
};


#endif //FHOBOTS_FIELD_HPP
