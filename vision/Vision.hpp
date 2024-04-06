#ifndef FHOBOTS_VISION_HPP
#define FHOBOTS_VISION_HPP

#include "opencv2/opencv.hpp"
#include "ColorDetection.hpp"
#include "opencv2/core/ocl.hpp"
#include "IVision.hpp"
#include "../logging/Logger.hpp"
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include "../config/Config.hpp"
#include "../model/Vector2D.hpp"
#include "../model/WorldModel.hpp"

enum Position
{
    POS_BALL,
    POS_GOALKEEPER,
    POS_DEFENDER,
    POS_ATTACKER,
    POS_TOTAL
};

class Vision : public IVision
{
    public:

        Vision(int deviceNumber, Config * config);
        ~Vision();

        void adjustFieldPosition();
        void calibration();
        void calibrateColor(ColorDetection * color);

        void detectionColors();
        void show(bool resized = true);
        void show(cv::UMat frame);
        void readFrame(bool cropped = true);
        void drawLine(const Coordinates<double> p1, const Coordinates<double> p2, const cv::Scalar color);

    private:

        void updateRobot(const std::vector<Coordinates<double>>& colorIndividual, const std::vector<Coordinates<double>>& colorTeam, std::string role);
        void calculatePosRobot(const std::vector<Coordinates<double>>& colorIndividual, const std::vector<Coordinates<double>>& colorTeam, Robot * robot);
        void writeRectFields();
        void readRectFields();
        void drawInfo();
        void drawRole(std::string role, std::string colorString);
        cv::Point toPixel(Vector2D v);
        void drawSafeZone();
        
        Logger * logger;
        cv::Mat _gpuFrame;
        cv::VideoCapture _videoCapture;
        Config * config;
        std::unordered_map <std::string, ColorDetection *> _colorItens;

};


#endif //FHOBOTS_VISION_HPP
