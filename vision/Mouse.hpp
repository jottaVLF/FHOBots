#ifndef FHOBOTS_MOUSE_HPP
#define FHOBOTS_MOUSE_HPP

#include <string>
#include <iostream>
#include <cstdlib>

#include "opencv2/opencv.hpp"
#include "../Coordinates.hpp"

class Mouse {

    public:
        Mouse();
        ~Mouse();

        bool isSelected();
        cv::Rect getRect();
        cv::Point getPosition();
        void setRect(const cv::Rect rect);
        void setWindow(const std::string nameFrame);

    private:

        void mouseHandler(int& event, int& x, int& y, int& flags);
        static void mouseCallback(int event, int x, int y, int flags, void *ptr);

        bool _isSelected;
        bool _isDrag;

        Coordinates<int> _pos;
        cv::Point _point1;
        cv::Point _point2;

        cv::Rect _rectangle;
};


#endif //FHOBOTS_MOUSE_HPP
