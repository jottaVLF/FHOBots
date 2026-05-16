#include "Kalman2D.hpp"

Kalman2D::Kalman2D()
    : _filter(4, 2, 0), _measurement(2, 1, CV_32F), _initialized(false), _dt(1.0), _errorCovariance(10.0)
{
    configure(1e-2, 4.0, 10.0, 1.0);
}

void Kalman2D::configure(double processNoise, double measurementNoise, double errorCovariance, double dt)
{
    _dt = dt;
    _errorCovariance = errorCovariance;

    _filter.init(4, 2, 0);
    _filter.transitionMatrix = (cv::Mat_<float>(4, 4) <<
        1, 0, static_cast<float>(_dt), 0,
        0, 1, 0, static_cast<float>(_dt),
        0, 0, 1, 0,
        0, 0, 0, 1);

    _filter.measurementMatrix = (cv::Mat_<float>(2, 4) <<
        1, 0, 0, 0,
        0, 1, 0, 0);

    cv::setIdentity(_filter.processNoiseCov, cv::Scalar::all(processNoise));
    cv::setIdentity(_filter.measurementNoiseCov, cv::Scalar::all(measurementNoise));
    cv::setIdentity(_filter.errorCovPost, cv::Scalar::all(errorCovariance));

    _filter.statePost = cv::Mat::zeros(4, 1, CV_32F);
    _filter.statePre = cv::Mat::zeros(4, 1, CV_32F);
    _measurement = cv::Mat::zeros(2, 1, CV_32F);
    _initialized = false;
}

void Kalman2D::reset()
{
    _filter.statePost = cv::Mat::zeros(4, 1, CV_32F);
    _filter.statePre = cv::Mat::zeros(4, 1, CV_32F);
    cv::setIdentity(_filter.errorCovPost, cv::Scalar::all(_errorCovariance));
    _initialized = false;
}

bool Kalman2D::isInitialized() const
{
    return _initialized;
}

void Kalman2D::init(double x, double y)
{
    _filter.statePost.at<float>(0) = static_cast<float>(x);
    _filter.statePost.at<float>(1) = static_cast<float>(y);
    _filter.statePost.at<float>(2) = 0.0f;
    _filter.statePost.at<float>(3) = 0.0f;
    _filter.statePost.copyTo(_filter.statePre);
    _initialized = true;
}

void Kalman2D::dampVelocity(double factor)
{
    if(!_initialized)
        return;

    _filter.statePost.at<float>(2) *= static_cast<float>(factor);
    _filter.statePost.at<float>(3) *= static_cast<float>(factor);
    _filter.statePost.copyTo(_filter.statePre);
}

void Kalman2D::predict()
{
    if(!_initialized)
        return;

    _filter.predict();
    _filter.statePre.copyTo(_filter.statePost);
    _filter.errorCovPre.copyTo(_filter.errorCovPost);
}

void Kalman2D::correct(double x, double y)
{
    if(!_initialized)
    {
        init(x, y);
        return;
    }

    _measurement.at<float>(0) = static_cast<float>(x);
    _measurement.at<float>(1) = static_cast<float>(y);
    _filter.correct(_measurement);
}

Vector2D Kalman2D::position() const
{
    if(!_initialized)
        return Vector2D(-1, -1);

    return Vector2D(_filter.statePost.at<float>(0), _filter.statePost.at<float>(1));
}

Vector2D Kalman2D::velocity() const
{
    if(!_initialized)
        return Vector2D(0, 0);

    return Vector2D(_filter.statePost.at<float>(2), _filter.statePost.at<float>(3));
}

Vector2D Kalman2D::predictedPosition(double horizonFrames) const
{
    if(!_initialized)
        return Vector2D(-10, -10);

    return Vector2D(
        _filter.statePost.at<float>(0) + _filter.statePost.at<float>(2) * horizonFrames,
        _filter.statePost.at<float>(1) + _filter.statePost.at<float>(3) * horizonFrames
    );
}
