#include "FakeVision.hpp"

FakeVision::FakeVision(){
    _gui = cv::Mat::zeros(600, 800, CV_8UC4);
    this->selectedRobot = NULL;
    this->ballSelected = false;
    this->_changed = false;
    this->_halfTime = false;
}

void FakeVision::adjustFieldPosition(){

    Global::areaGoalAttack.x  = 165*800/170. - widthToPixel(10)/2;
    Global::areaGoalAttack.y  = 300 - heigthToPixel(40)/2;
    Global::areaGoalAttack.height = heigthToPixel(40);
    Global::areaGoalAttack.width = widthToPixel(10);
    
    Global::areaGoalDeffend.x  = 5*800/170. - widthToPixel(10)/2;
    Global::areaGoalDeffend.y  = 300 - heigthToPixel(40)/2;
    Global::areaGoalDeffend.height = heigthToPixel(40);
    Global::areaGoalDeffend.width = widthToPixel(10);
    
    Global::areaToAttack.x  = 152.5*800/170. - widthToPixel(15)/2;
    Global::areaToAttack.y  = 300 - heigthToPixel(70)/2;
    Global::areaToAttack.height = heigthToPixel(70);
    Global::areaToAttack.width = widthToPixel(15);
    
    Global::areaToDeffend.x  = 17.5*800/170. - widthToPixel(15)/2;
    Global::areaToDeffend.y  = 300 - heigthToPixel(70)/2;
    Global::areaToDeffend.height = heigthToPixel(70);
    Global::areaToDeffend.width = widthToPixel(15);

    Global::fieldRect.x = 10;
    Global::fieldRect.y = 0;
    Global::fieldRect.width = 800;
    Global::fieldRect.height = 600;

    Global::eAreaAttack = AREA_ATTACK_RIGHT;
    Global::eAreaDeffend= AREA_DEFFEND_LEFT;
}
    
void FakeVision::calibration(){

    cv::namedWindow("Fake game");
    cv::setMouseCallback("Fake game", FakeVision::mouseActions, this);

    Global::attacker.setPosition(300, 300);
    Global::deffender.setPosition(400, 400);
    Global::goalkeeper.setPosition(500, 500);
    Global::attacker.setOrientationRobot(cos(M_PI/6.), sin(M_PI/6.));
    Global::deffender.setOrientationRobot(cos(-M_PI/6.), sin(-M_PI/6.));
    Global::goalkeeper.setOrientationRobot(cos(-3*M_PI/6.), sin(-3*M_PI/6.));
    Global::ball.x = 300;
    Global::ball.y = 10;
}

void FakeVision::detectionColors(){
    Global::ballPos.set(Global::ball.x, Global::ball.y);
    Global::ballVel.set(0, 0);
}

void FakeVision::show(){
    _gui = cv::Scalar(0, 0, 0);
    drawField();
    // robots Orientation 
    cv::line(_gui, toPixel(Global::attacker.getPosition()), cv::Point(Global::attacker.getPosition().x + 50* Global::attacker.getOrientation().x, Global::attacker.getPosition().y + 50* Global::attacker.getOrientation().y), cv::Scalar(255,255, 255, 1), 1, cv::LINE_4);
    cv::line(_gui, toPixel(Global::deffender.getPosition()), cv::Point(Global::deffender.getPosition().x + 50* Global::deffender.getOrientation().x, Global::deffender.getPosition().y + 50* Global::deffender.getOrientation().y), cv::Scalar(255,255, 255, 1), 1, cv::LINE_4);
    cv::line(_gui, toPixel(Global::goalkeeper.getPosition()), cv::Point(Global::goalkeeper.getPosition().x + 50* Global::goalkeeper.getOrientation().x, Global::goalkeeper.getPosition().y + 50* Global::goalkeeper.getOrientation().y), cv::Scalar(255,255, 255, 1), 1, cv::LINE_4);
    // robots
    cv::circle(_gui, toPixel(Global::attacker.getPosition()), heigthToPixel(3.5), cv::Scalar(255, 70, 168, 0.8), 10);
    cv::circle(_gui, toPixel(Global::deffender.getPosition()), heigthToPixel(3.5), cv::Scalar(0, 170, 0, 0.8), 10);
    cv::circle(_gui, toPixel(Global::goalkeeper.getPosition()), heigthToPixel(3.5), cv::Scalar(0, 0, 168, 0.8), 10);
    // robots state
    cv::putText(_gui, Global::attacker.getMessage(),cv::Point(Global::attacker.getPosition().x, Global::attacker.getPosition().y - 25) ,cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(255,255,255, 1), 1);
    cv::putText(_gui, Global::deffender.getMessage(),cv::Point(Global::deffender.getPosition().x, Global::deffender.getPosition().y - 25) ,cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(255,255,255, 1), 1);
    cv::putText(_gui, Global::goalkeeper.getMessage(),cv::Point(Global::goalkeeper.getPosition().x, Global::goalkeeper.getPosition().y - 25) ,cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(255,255,255, 1), 1);
    // objectives
    cv::line(_gui, toPixel(Global::attacker.getPosition()), toPixel(Global::attacker.objPos), cv::Scalar(255, 70, 168, 0.8), 1, cv::LINE_4);
    cv::line(_gui, toPixel(Global::deffender.getPosition()), toPixel(Global::deffender.objPos), cv::Scalar(0, 170, 0, 0.8), 1, cv::LINE_4);
    cv::line(_gui, toPixel(Global::goalkeeper.getPosition()), toPixel(Global::goalkeeper.objPos), cv::Scalar(0, 0, 168, 0.8), 1, cv::LINE_4);
    // ball
    cv::circle(_gui, toPixel(Global::ball), 2, cv::Scalar(5, 90, 255, 0.8), 7);
    if(this->selectedRobot != NULL)
        cv::circle(_gui, toPixel(this->selectedRobot->getPosition()), 15, cv::Scalar(255, 0, 255, 0.8), 1);
    if(this->ballSelected)
        cv::circle(_gui, toPixel(Global::ball), 15, cv::Scalar(255, 0, 255, 0.8), 1);
    cv::imshow("Fake game", _gui);
}

cv::Point FakeVision::toPixel(double cmX, double cmY){
    double x = cmX * 800 / 170.;
    double y = cmY * 600 / 130.;
    return cv::Point(x, y);
}

cv::Point FakeVision::toPixel(Vector2D v){
    return cv::Point(v.x, v.y);
}

void FakeVision::drawField(){

    // center
    cv::line(_gui, toPixel(170/2, 0), toPixel(170/2, 130), cv::Scalar(255,255, 255, 0.5), 1, cv::LINE_4);
    // goals lines
    cv::line(_gui, toPixel(10, 0), toPixel(10, 130), cv::Scalar(255,255, 255, 0.5), 1, cv::LINE_4);
    cv::line(_gui, toPixel(160, 0), toPixel(160, 130), cv::Scalar(255,255, 255, 0.5), 1, cv::LINE_4);
    // goals sides
    cv::line(_gui, toPixel(160, 130/2 + 20), toPixel(170, 130/2 + 20), cv::Scalar(255,255, 255, 0.5), 1, cv::LINE_4);
    cv::line(_gui, toPixel(160, 130/2 - 20), toPixel(170, 130/2 - 20), cv::Scalar(255,255, 255, 0.5), 1, cv::LINE_4);
    cv::line(_gui, toPixel(0, 130/2 + 20), toPixel(10, 130/2 + 20), cv::Scalar(255,255, 255, 0.5), 1, cv::LINE_4);
    cv::line(_gui, toPixel(0, 130/2 - 20), toPixel(10, 130/2 - 20), cv::Scalar(255,255, 255, 0.5), 1, cv::LINE_4);
    // areas
    cv::rectangle(_gui, toPixel(10, 130/2 + 35), toPixel(25, 130/2 - 35), cv::Scalar(255,255, 255, 0.5), 1, cv::LINE_4);
    cv::rectangle(_gui, toPixel(145, 130/2 + 35), toPixel(160, 130/2 - 35), cv::Scalar(255,255, 255, 0.5), 1, cv::LINE_4);
}

int FakeVision::heigthToPixel(double cm){
    return (int) (cm * 600/130.);
}


int FakeVision::widthToPixel(double cm){
    return (int) (cm * 800/170.);
}

void FakeVision::mouseActions(int event, int x, int y, int flags, void * userdata){
    FakeVision * self = (FakeVision *) userdata;
    if(event == cv::EVENT_LBUTTONDOWN){
        
        if(self->selectedRobot != NULL){
            if(!self->_changed)
                self->_changed = true;
            self->selectedRobot->setPosition(x, y);
            self->selectedRobot = NULL;
        }else{
            if(self->isClicked(Global::attacker.getPosition(), x, y))
                self->selectedRobot = &Global::attacker;
            else if(self->isClicked(Global::deffender.getPosition(), x, y))
                self->selectedRobot = &Global::deffender;
            else if(self->isClicked(Global::goalkeeper.getPosition(), x, y))
                self->selectedRobot = &Global::goalkeeper;
        } 
        
        if(!self->ballSelected && self->selectedRobot == NULL){
            if(self->isClicked(Global::ball, x, y)){
                self->ballSelected = true;
            }
        }else if(self->ballSelected){
            if(!self->_changed)
                self->_changed = true;
            Global::ball.x = x;
            Global::ball.y = y;
            self->ballSelected = false;
        }
    }else if(event == cv::EVENT_MBUTTONDOWN){
        if(self->selectedRobot != NULL){
            int xr = self->selectedRobot->getPosition().x;
            int yr = self->selectedRobot->getPosition().y;
            double mag = sqrt(pow(x - xr, 2) + pow(y - yr, 2));
            self->selectedRobot->setOrientationRobot((x - xr)/mag, (y-yr)/mag);
        }
    }else if(flags == cv::EVENT_FLAG_CTRLKEY){
        self->selectedRobot = NULL;
        self->ballSelected  = false;
    }else if(flags == cv::EVENT_FLAG_SHIFTKEY && !self->_halfTime){
        WorldModel::halfTime();
        self->_halfTime = true;
    }

}

bool FakeVision::isClicked(Vector2D v, int x, int y){
    return pow(x - v.x, 2) + pow(y - v.y, 2) < 100;
}