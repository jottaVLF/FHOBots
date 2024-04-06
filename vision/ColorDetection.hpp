#ifndef FHOBOTS_COLORDETECTION_HPP
#define FHOBOTS_COLORDETECTION_HPP

#include "CustomTrackbar.hpp"
#include "../Global.hpp"
#include <vector>
#include "opencv2/opencv.hpp"

class ColorDetection
{
    public:
        ColorDetection(const std::string name, cv::Mat* gpuFrame);
        ~ColorDetection();

        void initCalibrate();
        void exitCalibrate();
        void showTrackBar();

        void detect(bool onCalibration = false);
        void calibrate();

        std::vector<Coordinates<double>>& getPositionsColor();

        std::string getWindowName();
        cv::UMat getFilterState();

    private:
        void filter();
        void morphologicOperations();
        void setPosition(cv::InputArray arrayContours);

        CustomTrackbar _trackbar;

        std::vector<Coordinates<double>> _pos;
        cv::UMat _mask; /// Imagem tratada com a cor
        cv::UMat _hsv;

        cv::Mat* _gpuFrameOriginal;
};


#endif //FHOBOTS_COLORDETECTION_HPP
