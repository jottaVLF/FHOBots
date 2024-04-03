#ifndef FAKE_VISION_HPP
#define FAKE_VISION_HPP

#include "IVision.hpp"
#include "../Global.hpp"
#include "../model/Vector2D.hpp"
#include "../model/WorldModel.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <cmath>
#include <cstdlib>

class FakeVision : public IVision{
public:
    void detectionColors(); 
    void adjustFieldPosition();
    void calibration();
    void show();
    FakeVision();
private:
    cv::Mat _gui;
    cv::Point toPixel(double cmX, double cmY);
    cv::Point toPixel(Vector2D v);
    int heigthToPixel(double cm);
    int widthToPixel(double cm);
    static void mouseActions(int event, int x, int y, int flags, void * userdata);
    void drawField();
    Robot * selectedRobot;
    bool ballSelected;
    bool isClicked(Vector2D v, int x, int y);
    bool _changed;
    bool _halfTime;
};



#endif