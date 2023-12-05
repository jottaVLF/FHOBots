#ifndef FHOBOTS_CUSTOMTRACKBAR_HPP
#define FHOBOTS_CUSTOMTRACKBAR_HPP
#include <string>
#include "opencv.hpp"

class CustomTrackbar {

    public:
        CustomTrackbar(const std::string name);
        ~CustomTrackbar();

        std::string& getName();

        int getAreaMax();
        int getAreaMin();
        void show();

        cv::Scalar getColorHigh();
        cv::Scalar getColorLow();

        void initTrackBar();
        void exitTrackBar();

    private:

        void loadColorHigh(const cv::Scalar color);
        void loadColorLow(const cv::Scalar color);

        std::string _name;
        cv::Mat _mat;

        int _areaMax;
        int _areaMin;

        int _hsvHigh[3]; /// Preciso usar isso em vez de scalar, pq a trackbar n aceita o scalar
        int _hsvLow[3]; /// Idem

        static const std::string _trackBarHigh[3];
        static const std::string _trackBarLow[3];
};


#endif //FHOBOTS_CUSTOMTRACKBAR_HPP
