#ifndef SIM_VISION_H
#define SIM_VISION_H

#include "IVision.hpp"
#include "../Global.hpp"
#include "../model/Vector2D.hpp"
#include "../model/Robot.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cmath>
#include "../config/Config.hpp"
#include "opencv2/opencv.hpp"
// Importação referente ao protobuf
#include "../pb/packet.pb.h"
#include "../pb/common.pb.h"


class SimVision : public IVision{
private:
    int socketFd;
    sockaddr_in server;
    socklen_t sizeSocket;
    char buffer[4096];
    Config * config;
    double toPixelY(double y);
    double toPixelX(double x);
    void setArea(Area & area, double x, double y, double width, double height, bool isPixel = false);
    void setOrientationVector(Robot * robot, double angle);
public:
    SimVision(Config * config, std::string ip, int port);
    ~SimVision();
    void detectionColors(); 
    void adjustFieldPosition();
    void calibration();
};

#endif