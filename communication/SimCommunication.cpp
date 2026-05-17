#include "SimCommunication.hpp"
#include <cmath>

SimCommunication::SimCommunication(Config * config, std::string ip, int port) : SimCommunication(config, ip, port, config->simMaxSpeed)
{}

SimCommunication::SimCommunication(Config * config, std::string ip, int port, double maxSpeed){
    this->socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    this->server.sin_family = AF_INET;
    this->server.sin_port = htons(port);
    inet_aton(ip.c_str(), &this->server.sin_addr);
    this->sizeSocket = sizeof(this->server);

    this->config = config;
    this->_maxSpeed = maxSpeed;
    this->_smoothing = config->simSmoothing;
    this->_deadband = config->simDeadband;
    for(int i = 0; i < 6; i++){
        this->velocities[i] = 0;
        this->targetVelocities[i] = 0;
    }
}

SimCommunication::~SimCommunication(){
    close(this->socketFd);
}

void SimCommunication::writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight, const bool reverseLeft, const bool reverseRight){
    double leftVelocity  = toSimSpeed(pwmLeft);
    double rightVelocity = toSimSpeed(pwmRight);

    this->targetVelocities[2*index]     = reverseLeft  ? -leftVelocity  : leftVelocity;
    this->targetVelocities[2*index + 1] = reverseRight ? -rightVelocity : rightVelocity;
}

void SimCommunication::sendMessage(){
    bool isYellowTeam = this->config->teamColor == "yellow";
    for(int i = 0; i < 6; i++)
        this->velocities[i] = smoothVelocity(this->velocities[i], this->targetVelocities[i]);
    for(int i = 0; i < 3; i++)
        this->sendCommandToRobot(this->velocities[2*i], this->velocities[2 * i + 1], isYellowTeam, i);
}

void SimCommunication::stopAll(){
    for(int i = 0; i < 6; i++){
        this->velocities[i] = 0;
        this->targetVelocities[i] = 0;
    }
}

void SimCommunication::configureRobots(Config config){

}

void SimCommunication::sendCommandToRobot(double leftWheel, double rightWheel, bool isYellowTeam, int id){
    fira_message::sim_to_ref::Packet packet;
    fira_message::sim_to_ref::Command * command = packet.mutable_cmd()->add_robot_commands();
    command->set_yellowteam(isYellowTeam);
    command->set_id(id);
    command->set_wheel_left(leftWheel);
    command->set_wheel_right(rightWheel);
    packet.SerializeToArray(this->buffer, 4096);
    sendto(this->socketFd, this->buffer, 4096, 0, (sockaddr *) &this->server, this->sizeSocket);
}

int SimCommunication::toSimSpeed(int pwm){
    return _maxSpeed / 255. * pwm;
}

double SimCommunication::smoothVelocity(double current, double target){
    double next = current + (target - current) * _smoothing;
    if(std::abs(next) < _deadband)
        return 0;
    return next;
}
