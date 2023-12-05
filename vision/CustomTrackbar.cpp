#include "CustomTrackbar.hpp"
#include <fstream>
#include <iostream>

const std::string CustomTrackbar::_trackBarHigh[3] = {"hh", "sh", "vh"};
const std::string CustomTrackbar::_trackBarLow[3] = {"hl", "sl", "vl"};

void loadColors(const std::string pathFile, cv::Scalar& high, cv::Scalar& low)
{
    std::ifstream file(pathFile);
    if(file.is_open())
    {
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                if(i == 0)
                    file >> high[j];
                else
                    file >> low[j];
            }
        }

        file.close();
        return;
    }

    std::cerr << "File: " << pathFile << " does not exist!" << std::endl;
}

void saveColors(const std::string pathFile, cv::Scalar high, cv::Scalar low)
{
    std::ofstream file(pathFile);
    if(file.is_open())
    {
        std::cout << "Saving color info in "<< pathFile << std::endl;
        file << high[0] << "\n";
        file << high[1] << "\n";
        file << high[2] << "\n";
        file << low[0] << "\n";
        file << low[1] << "\n";
        file << low[2] << "\n";
        std::cout << "Closing file "<< pathFile << std::endl;
        file.close();
    } else
        std::cout << "FILE OPENING ERROR!!!\n\n";
}

CustomTrackbar::CustomTrackbar(const std::string name) : _name(name), _mat(1, 300, CV_8UC1), _areaMax(0), _areaMin(0)
{
    cv::Scalar high;
    cv::Scalar low;

    loadColors("files/colors/" + _name + ".txt", high, low);
    loadColorHigh(high);
    loadColorLow(low);
}

CustomTrackbar::~CustomTrackbar()
{}

std::string& CustomTrackbar::getName()
{
    return _name;
}

int CustomTrackbar::getAreaMax()
{
    return _areaMax;
}

int CustomTrackbar::getAreaMin()
{
    return _areaMin;
}

void CustomTrackbar::show()
{
    cv::imshow(_name + " track", _mat);
}

cv::Scalar CustomTrackbar::getColorHigh()
{
    return cv::Scalar(_hsvHigh[0], _hsvHigh[1], _hsvHigh[2]);
}

cv::Scalar CustomTrackbar::getColorLow()
{
    return cv::Scalar(_hsvLow[0], _hsvLow[1], _hsvLow[2]);
}

void CustomTrackbar::initTrackBar()
{
    const std::string nameWindow = _name + " track";
    cv::namedWindow(nameWindow);

    for(int i = 0; i < 3; i++)
    {
        if(i == 0)
        {
            cv::createTrackbar(CustomTrackbar::_trackBarHigh[i], nameWindow, &_hsvHigh[i], 180);
            cv::createTrackbar(CustomTrackbar::_trackBarLow[i], nameWindow, &_hsvLow[i], 180);
        }
        else
        {
            cv::createTrackbar(CustomTrackbar::_trackBarHigh[i], nameWindow, &_hsvHigh[i], 255);
            cv::createTrackbar(CustomTrackbar::_trackBarLow[i], nameWindow, &_hsvLow[i], 255);
        }
    }

    _areaMax = 500;
    cv::createTrackbar("Max Area", nameWindow, &_areaMax, 1000);
    cv::createTrackbar("Min Area", nameWindow, &_areaMin, 1000);
}

void CustomTrackbar::exitTrackBar()
{
    std::cout << "Exiting " << _name << std::endl;
    saveColors("files/colors/" + _name + ".txt", cv::Scalar(_hsvHigh[0], _hsvHigh[1], _hsvHigh[2]), cv::Scalar(_hsvLow[0], _hsvLow[1], _hsvLow[2]));
    cv::destroyWindow(_name + " track");
}

void CustomTrackbar::loadColorHigh(const cv::Scalar color)
{
    _hsvHigh[0] = color[0];
    _hsvHigh[1] = color[1];
    _hsvHigh[2] = color[2];
}

void CustomTrackbar::loadColorLow(const cv::Scalar color)
{
    _hsvLow[0] = color[0];
    _hsvLow[1] = color[1];
    _hsvLow[2] = color[2];
}
