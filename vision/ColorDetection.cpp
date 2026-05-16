#include "ColorDetection.hpp"

ColorDetection::ColorDetection(const std::string name, cv::Mat* gpuFrame) : _trackbar(name), _gpuFrameOriginal(gpuFrame)
{}

ColorDetection::~ColorDetection()
{}

void ColorDetection::initCalibrate()
{
    _trackbar.initTrackBar();
}

void ColorDetection::exitCalibrate()
{
    _trackbar.exitTrackBar();
}

void ColorDetection::showTrackBar()
{
    _trackbar.show();
}


void ColorDetection::detect(bool onCalibration)
{
    if(_pos.size() > 0)
        _pos.clear(); 

    filter();
    morphologicOperations();
    if(getWindowName() == "ball")
        cv::imshow("ball", _mask);
    std::vector<std::vector<cv::Point>> contours;
    cv::UMat hierarchy;

    cv::findContours(_mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0,0));

    cv::Scalar cor;

    int sizeCountours = contours.size();
    if(sizeCountours > 0)
    {
        for(int i = 0; i < sizeCountours; i++)
        {
            double area = cv::contourArea(contours[i]);

            if(area >= _trackbar.getAreaMin() && area <= _trackbar.getAreaMax())
            {
                if(getWindowName()=="yellow")
                    cor = cv::Scalar(4, 162, 187);
               else if(getWindowName()=="blue")
                    cor = cv::Scalar(255, 0, 0);
               else if(getWindowName()=="green")
                    cor = cv::Scalar(0, 255, 0);
               else if(getWindowName()=="red")
                    cor = cv::Scalar(0, 0, 255);
               else if(getWindowName()=="purple")
                    cor = cv::Scalar(212, 15, 146);
               else if(getWindowName()=="brown")
                    cor = cv::Scalar(255, 255, 255);
               else if(getWindowName()=="ball")
                   cor = cv::Scalar(0, 165, 255);

              /*if(!onCalibration)
                    cv::drawContours(*_gpuFrameOriginal, contours, i, cor, 3, 8, hierarchy);*/
               setPosition(contours[i]);
            }
        }
    }
}

void ColorDetection::calibrate()
{
    filter();
}

std::vector<Coordinates<double>>& ColorDetection::getPositionsColor()
{
    detect();
    return _pos;
}

std::string ColorDetection::getWindowName()
{
    return _trackbar.getName();
}

cv::UMat ColorDetection::getFilterState()
{
    return _mask;
}

void ColorDetection::filter()
{
    if(_gpuFrameOriginal != NULL)
    {
        cv::cvtColor(*_gpuFrameOriginal, _hsv, cv::COLOR_BGR2HSV);
        cv::inRange(_hsv, _trackbar.getColorLow(), _trackbar.getColorHigh(), _mask);
    }
}

void ColorDetection::morphologicOperations()
{
    cv::Mat erodeElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
    cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(8,8));

    cv::erode(_mask, _mask, erodeElement);
    cv::dilate(_mask, _mask, dilateElement);
}

void ColorDetection::setPosition(cv::InputArray arrayContours)
{
    Coordinates<double> c = {0, 0};
    cv::Moments m = cv::moments(arrayContours);
    if(m.m00 != 0)
    {
        c.x = m.m10 / m.m00;
        c.y = m.m01 / m.m00;
        _pos.push_back(c);
    }
}
