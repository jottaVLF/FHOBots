#ifndef SIM_COMMUNICATION_H
#define SIM_COMMUNICATION_H

#include "ICommunication.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "../pb/packet.pb.h"
#include "../pb/replacement.pb.h"
#include "../pb/command.pb.h"

class SimCommunication : public ICommunication{

private:
    int socketFd;
    char buffer[4096];
    socklen_t sizeSocket;
    sockaddr_in server;
    Config * config;
    void sendCommandToRobot(double leftWheel, double rightWheel, bool isYellowTeam, int id);
    double velocities[6];
    int toSimSpeed(int pwm);
    double _maxSpeed;
public:
    SimCommunication(Config * config, std::string ip, int port, double maxSpeed);
    ~SimCommunication();

    void writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight, const bool reverseLeft, const bool reverseRight);
    void sendMessage();
    void stopAll();
    void configureRobots(Config config);
};

#endif
