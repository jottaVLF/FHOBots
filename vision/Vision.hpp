#ifndef FHOBOTS_VISION_HPP
#define FHOBOTS_VISION_HPP

#include "opencv2/opencv.hpp"
#include "ColorDetection.hpp"
#include "opencv2/core/ocl.hpp"
#include <opencv2/imgproc.hpp>
#include "IVision.hpp"
#include "../logging/Logger.hpp"
#include <cstring>
#include <string>
#include <cstdlib>
#include <unordered_map>
#include "../config/Config.hpp"
#include "../model/Vector2D.hpp"
#include "../model/WorldModel.hpp"
#include "Kalman2D.hpp"

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
        void calibratePreProcess();
        
    private:

        void updateRobot(const std::vector<Coordinates<double>>& colorIndividual, const std::vector<Coordinates<double>>& colorTeam, std::string role);
        void calculatePosRobot(const std::vector<Coordinates<double>>& colorIndividual, const std::vector<Coordinates<double>>& colorTeam, Robot * robot);
        void writeRectFields();
        void writeCoeficients();
        void readRectFields();
        void readCoeficients();
        void drawInfo();
        void drawRole(std::string role, std::string colorString);
        cv::Point toPixel(Vector2D v);
        void drawSafeZone();
        void drawVelocityVector(Vector2D origin, Vector2D velocity, cv::Scalar color);
        void drawBallKalmanDebug();
        void calibratePreProcessVideo();
        void markRobotDetectionLost(Robot * robot);
        void updateRobotKalman(Robot * robot, double measuredX, double measuredY);
        void updateBallKalman(const std::vector<Coordinates<double>>& ballDetections);
        Vector2D keepBallPredictionInsideField(Vector2D predicted, Vector2D filteredPosition);
        
        Logger * logger;
        cv::Mat _gpuFrame;
        cv::VideoCapture _videoCapture;
        Config * config;
        std::unordered_map <std::string, ColorDetection *> _colorItens;
        int _cameraCoeficients[4];
        Vector2D _lastTrackedPosition[3];
        bool _hasTrackedPosition[3];
        bool _trackingHeld[3];
        int _lostFrames[3];
        Kalman2D _robotKalman[3];
        Vector2D _lastRobotMeasurement[3];
        Vector2D _lastRobotFiltered[3];
        bool _hasRobotMeasurement[3];
        bool _hasRobotFilter[3];
        Kalman2D _ballKalman;
        Vector2D _lastBallMeasurement;
        Vector2D _lastBallFiltered;
        Vector2D _lastBallPrediction;
        bool _hasBallMeasurement;
        bool _hasBallFilter;
        bool _ballPredictionOnly;
        int _ballLostFrames;
};

#endif //FHOBOTS_VISION_HPP
