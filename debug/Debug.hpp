#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "../Global.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
#include <cmath>
#include <cstring>

class Debug{
private:
    bool is3v3;
    cv::Mat debug;
    void drawRobot(Robot * robot, bool isFhobotsYellow);
    void drawAdversary(bool isFhobotsYellow);
    void drawField3v3();
    cv::Point toPixel(double x, double y);
    double toPixelY(double y);
    double toPixelX(double x);
    bool _isFhobotsYellow;
public:
    Debug(bool isFhobotsYellow, bool is3v3 = true);
    void show();
};

#endif
