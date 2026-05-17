#include "SimVision.hpp"

SimVision::SimVision(Config * config, std::string ip, int port){
    this->socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    this->server.sin_family = AF_INET;
    this->server.sin_port = htons(port);
    inet_aton(ip.c_str(), &this->server.sin_addr);
    this->sizeSocket = sizeof(this->server);
    int enable = 1;
    setsockopt(this->socketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    bind(this->socketFd, (sockaddr * ) &this->server, this->sizeSocket);
    this->config = config;
    std::cout << "Connected to simulator" << std::endl;
    for(int i = 0; i < 3; i++){
        Vector2D vec;
        Global::enemyTeam.push_back(vec);
    }
}

SimVision::~SimVision(){
    close(this->socketFd);
}

void SimVision::detectionColors(){
    int nbytes = recvfrom(this->socketFd, this->buffer, 4096, 0, (sockaddr *) &this->server, &this->sizeSocket);
    if(nbytes > 0){
        fira_message::sim_to_ref::Environment environment;
        environment.ParseFromArray(this->buffer, 4096);
        Global::ball.set(toPixelX(environment.frame().ball().x()), toPixelY(environment.frame().ball().y()));
        Global::ballVel.set(environment.frame().ball().vx(), environment.frame().ball().vy());
        if(this->config->teamColor == "blue"){
            Global::goalkeeper.setPosition(toPixelX(environment.frame().robots_blue(0).x()), toPixelY(environment.frame().robots_blue(0).y()));
            setOrientationVector(&Global::goalkeeper, environment.frame().robots_blue(0).orientation());
            Global::deffender.setPosition(toPixelX(environment.frame().robots_blue(1).x()), toPixelY(environment.frame().robots_blue(1).y()));
            setOrientationVector(&Global::deffender, environment.frame().robots_blue(1).orientation());
            Global::attacker.setPosition(toPixelX(environment.frame().robots_blue(2).x()), toPixelY(environment.frame().robots_blue(2).y()));
            setOrientationVector(&Global::attacker, environment.frame().robots_blue(2).orientation());

            for(int i = 0; i < 3; i++)
                Global::enemyTeam[i].set(toPixelX(environment.frame().robots_yellow(i).x()), toPixelY(environment.frame().robots_yellow(i).y()));
        }else{
            Global::goalkeeper.setPosition(toPixelX(environment.frame().robots_yellow(0).x()), toPixelY(environment.frame().robots_yellow(0).y()));
            setOrientationVector(&Global::goalkeeper, environment.frame().robots_yellow(0).orientation());
            Global::deffender.setPosition(toPixelX(environment.frame().robots_yellow(1).x()), toPixelY(environment.frame().robots_yellow(1).y()));
            setOrientationVector(&Global::deffender, environment.frame().robots_yellow(1).orientation());
            Global::attacker.setPosition(toPixelX(environment.frame().robots_yellow(2).x()), toPixelY(environment.frame().robots_yellow(2).y()));
            setOrientationVector(&Global::attacker, environment.frame().robots_yellow(2).orientation());

            for(int i = 0; i < 3; i++)
                Global::enemyTeam[i].set(toPixelX(environment.frame().robots_blue(i).x()), toPixelY(environment.frame().robots_blue(i).y()));
        }
    }
}

void SimVision::adjustFieldPosition(){
    setArea(Global::fieldRect, 0, 0, 800, 600, true);
    if(this->config->teamColor == "blue"){
        setArea(Global::areaToDeffend, -0.75, 0.35, (-0.6 + 0.75), (2*0.35));
        setArea(Global::areaToAttack, 0.6, 0.35, (0.75 - 0.6), (2*0.35));
        setArea(Global::areaGoalDeffend, -0.85, 0.22, 0.1, 0.4);
        setArea(Global::areaGoalAttack, 0.85, 0.22, 0.1, 0.4);
        Global::eAreaDeffend = AREA_DEFFEND_LEFT;
        Global::eAreaAttack = AREA_DEFFEND_RIGHT;
    }else{
        setArea(Global::areaToAttack, -0.75, 0.35, (-0.6 + 0.75), (2*0.35));
        setArea(Global::areaToDeffend, 0.6, 0.35, (0.75 - 0.6), (2*0.35));
        setArea(Global::areaGoalAttack, -0.85, 0.22, 0.1, 0.4);
        setArea(Global::areaGoalDeffend, 0.85, 0.22, 0.1, 0.4);
        Global::eAreaDeffend = AREA_DEFFEND_RIGHT;
        Global::eAreaAttack = AREA_ATTACK_LEFT;
    }
    Global::centerGoalAttack.set(Global::areaToAttack.x + Global::areaToAttack.width / 2,
                                 Global::areaToAttack.y + Global::areaToAttack.height / 2);
}

void SimVision::setArea(Area & area, double x, double y, double width, double height, bool isPixel){
    if(!isPixel){
        x = toPixelX(x);
        y = toPixelY(y);
        width = width / 1.7 * Global::fieldRect.width;
        height = height / 1.3 * Global::fieldRect.height;
    }
    area.x = x;
    area.y = y;
    area.width = width;
    area.height = height;
}

void SimVision::calibration(){
}

double SimVision::toPixelY(double y){
    return -Global::fieldRect.height * (y - 0.65) / 1.3;
}

double SimVision::toPixelX(double x){
    return Global::fieldRect.width * (x + 0.85) / 1.7;
}

void SimVision::setOrientationVector(Robot * robot, double angle){
    robot->setOrientationRobot(cos(angle), -sin(angle));
}
