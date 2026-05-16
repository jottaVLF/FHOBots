#ifndef FHOBOTS_KALMAN2D_HPP
#define FHOBOTS_KALMAN2D_HPP

#include "../model/Vector2D.hpp"
#include <opencv2/video/tracking.hpp>

class Kalman2D
{
    public:
        Kalman2D();

        void configure(double processNoise, double measurementNoise, double errorCovariance, double dt = 1.0);
        void reset();
        bool isInitialized() const;

        void init(double x, double y);
        void dampVelocity(double factor);
        void predict();
        void correct(double x, double y);

        Vector2D position() const;
        Vector2D velocity() const;
        Vector2D predictedPosition(double horizonFrames) const;

    private:
        cv::KalmanFilter _filter;
        cv::Mat _measurement;
        bool _initialized;
        double _dt;
        double _errorCovariance;
};

#endif
