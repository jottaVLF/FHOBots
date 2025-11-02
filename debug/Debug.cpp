#include "Debug.hpp"

Debug::Debug(bool is3v3){
    this->debug = cv::Mat::zeros(Global::fieldRect.height, Global::fieldRect.width, CV_8UC4);
    cv::namedWindow("Debug");
    this->is3v3 = is3v3;
}


void Debug::show(){
    this->debug = cv::Scalar(0, 0, 0);
    drawField3v3();
    cv::circle(this->debug, toPixel(Global::ball.x, Global::ball.y), 2, cv::Scalar(0, 60, 255), 5);
    drawRobot(&Global::attacker, false);
    drawRobot(&Global::deffender, false);
    drawRobot(&Global::goalkeeper, false);
    drawAdversary(true);
    cv::imshow("Debug", this->debug);
    cv::waitKey(10);
}

double Debug::toPixelX(double x){
    if(this->is3v3)
        return Global::fieldRect.width*(x + 0.85)/ 1.7;
    return Global::fieldRect.width*(x + 1.25)/ 2.50;
}

double Debug::toPixelY(double y){
    if(this->is3v3)
        return -Global::fieldRect.height * (y - 0.65) / 1.3;
    return -Global::fieldRect.height * (y - 0.9) / 1.80;
}

cv::Point Debug::toPixel(double x, double y){
    double xPixel = toPixelX(x);
    double yPixel = toPixelY(y);

    return cv::Point(xPixel, yPixel);
}

void Debug::drawAdversary(bool isFhobotsYellow){
    cv::Scalar cor = isFhobotsYellow ? cv::Scalar(0, 255, 255) : cv::Scalar(255, 0, 0);
    for(int i = 0; i< 3; i++){
                cv::circle(this->debug, cv::Point(Global::enemyTeam[i].x, Global::enemyTeam[i].y), 2, cor, 5);
    }
}

void Debug::drawRobot(Robot * robot, bool isFhobotsYellow){
        double xOrientation = robot->getOrientation().x;
        double yOrientation = robot->getOrientation().y;
        cv::Scalar cor = isFhobotsYellow ? cv::Scalar(0, 255, 255) : cv::Scalar(255, 0, 0);
        char id[50], state[50];
        cv::line(this->debug,   
                 cv::Point(robot->getPosition().x, robot->getPosition().y), 
                 cv::Point(robot->getPosition().x + 30*xOrientation, robot->getPosition().y + 30*yOrientation), 
                 cv::Scalar(255, 255, 255)
                );
        cv::line(this->debug,   
                cv::Point(robot->getPosition().x,  robot->getPosition().y), 
                cv::Point(robot->getObjective().x, robot->getObjective().y), 
        cor);
        
        sprintf(id, "%d", robot->getPosMessage()); 
        
        cv::putText(this->debug, id, cv::Point(robot->getPosition().x - 15, robot->getPosition().y - 15), 
                cv::FONT_HERSHEY_COMPLEX_SMALL, 0.6, 
                cv::Scalar(255,255, 255));

        sprintf(state, "%s", robot->getMessage().c_str());
        cv::putText(this->debug, state, cv::Point(robot->getPosition().x, robot->getPosition().y - 15), 
                cv::FONT_HERSHEY_COMPLEX_SMALL, 0.6, 
                cv::Scalar(255,255, 255)); 
        cv::circle(this->debug, cv::Point(robot->getPosition().x, robot->getPosition().y), 2, cor, 5);

}

void Debug::drawField3v3(){
    cv::line(this->debug,   
            toPixel(0, 0.65), 
            toPixel(0, -0.65), 
            cv::Scalar(255, 255, 255));
    
    cv::line(this->debug,   
            toPixel(0.75, 0.65), 
            toPixel(0.75, -0.65), 
            cv::Scalar(255, 255, 255));

    cv::line(this->debug,   
            toPixel(-0.75, 0.65), 
            toPixel(-0.75, -0.65), 
            cv::Scalar(255, 255, 255));
    
    cv::line(this->debug,   
            toPixel(0.75, 0.22), 
            toPixel(0.85, 0.22), 
            cv::Scalar(255, 255, 255));
    
    cv::line(this->debug,   
            toPixel(0.75, -0.22), 
            toPixel(0.85, -0.22), 
            cv::Scalar(255, 255, 255));

    cv::line(this->debug,   
            toPixel(-0.75, 0.22), 
            toPixel(-0.85, 0.22), 
            cv::Scalar(255, 255, 255));
    
    cv::line(this->debug,   
            toPixel(-0.75, -0.22), 
            toPixel(-0.85, -0.22), 
            cv::Scalar(255, 255, 255));

    cv::line(this->debug,   
            toPixel(0.75,  0.35), 
            toPixel(0.6,  0.35), 
            cv::Scalar(255, 255, 255));

    cv::line(this->debug,   
            toPixel(0.75, -0.35), 
            toPixel(0.6,  -0.35), 
            cv::Scalar(255, 255, 255));

    cv::line(this->debug,   
            toPixel(0.6, 0.35), 
            toPixel(0.6, -0.35), 
            cv::Scalar(255, 255, 255));

    cv::line(this->debug,   
            toPixel(-0.75,  0.35), 
            toPixel(-0.6,  0.35), 
            cv::Scalar(255, 255, 255));

    cv::line(this->debug,   
            toPixel(-0.75, -0.35), 
            toPixel(-0.6,  -0.35), 
            cv::Scalar(255, 255, 255));

    cv::line(this->debug,   
            toPixel(-0.6, 0.35), 
            toPixel(-0.6, -0.35), 
            cv::Scalar(255, 255, 255));

    cv::circle(this->debug, toPixel(0, 0), 0.2*800/1.7, cv::Scalar(255,255,255), 1);
    cv::circle(this->debug, toPixel(0.375, 0.44), 1, cv::Scalar(255,255,255), 1.5);
    cv::circle(this->debug, toPixel(-0.375, 0.44), 1, cv::Scalar(255,255,255), 1.5);
    cv::circle(this->debug, toPixel(0.375, -0.44), 1, cv::Scalar(255,255,255), 1.5);
    cv::circle(this->debug, toPixel(-0.375, -0.44), 1, cv::Scalar(255,255,255), 1.5);
    cv::circle(this->debug, toPixel(0.375, 0), 1, cv::Scalar(255,255,255), 1.5);
    cv::circle(this->debug, toPixel(-0.375, 0), 1, cv::Scalar(255,255,255), 1.5);
    
}
