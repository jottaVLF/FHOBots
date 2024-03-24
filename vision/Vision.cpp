#include "Vision.hpp"
#include "../model/Field.hpp"
#include <sstream>
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace cv;

Vision::Vision(int deviceNumber, Config * config)
{
    bool success = _videoCapture.open(deviceNumber);
    if(!success){
        std::cout << "Could not open camera for device number " << deviceNumber << std::endl;
        exit(1);
    }
    if(!ocl::useOpenCL())
       ocl::setUseOpenCL(true);

    /*_videoCapture.set(3, 640);
    _videoCapture.set(4, 480);*/
    this->logger = new Logger("files/logs/Vision.log");
    this->config = config;
}

Vision::~Vision()
{
   destroyAllWindows();
}

void Vision::adjustFieldPosition()
{
    readFrame(false);

    Field f[5]{{"Board"},{"A. Deffend"},{"A. Attack"},{"G. Deffend"},{"G. Attack"}};

    readRectFields();
    f[0].setRect(Global::fieldRect);
    f[1].setRect(Global::areaToDeffend);
    f[2].setRect(Global::areaToAttack);
    f[3].setRect(Global::areaGoalDeffend);
    f[4].setRect(Global::areaGoalAttack);
    cv::namedWindow("Game original");

    for(int i = 0; i < 5; i++)
    {
        Global::bufferKeyboard = '0';
        while(Global::bufferKeyboard != 32)
        {
            
            readFrame(i != 0);
            if(i != 0)
                f[i].setWindowName("Game resized");
            f[i].draw(_gpuFrame);
            putText(_gpuFrame, f[i].getName(), Point(30, 50), 0, 1.5, Scalar(0, 0, 255), 5);
            f[i].setWindow();
            show(false);
            Global::bufferKeyboard = waitKey(16);
        }
        std::cout << i << std::endl;
        switch (i)
        {
            case 0:
                Global::fieldRect = f[i].getRect();
                break;
            case 1:
                Global::areaToDeffend = f[i].getRect();
                if(f[i].getRect().x < Global::fieldRect.width / 2)
                    Global::eAreaDeffend = AREA_DEFFEND_LEFT;
                else
                    Global::eAreaDeffend = AREA_ATTACK_RIGHT;
                break;

            case 2:
                Global::areaToAttack = f[i].getRect();
                if(f[i].getRect().x < Global::fieldRect.width / 2)
                    Global::eAreaAttack = AREA_ATTACK_LEFT;
                else
                    Global::eAreaAttack = AREA_ATTACK_RIGHT;
                Global::centerGoalAttack.set(Global::areaToAttack.x + Global::areaToAttack.width / 2,
                                             Global::areaToAttack.y + Global::areaToAttack.height / 2);
                break;

            case 3:
                Global::areaGoalDeffend = f[i].getRect();
                if(f[i].getRect().x < Global::fieldRect.width / 2)
                    Global::eAreaDeffend = AREA_DEFFEND_LEFT;
                else
                    Global::eAreaDeffend = AREA_DEFFEND_RIGHT;
                break;

            case 4:
                Global::areaGoalAttack = f[i].getRect();
                break;
        }
        if(i == 0){
           // cv::destroyWindow("Game original");
        }
    }

    writeRectFields();
    Global::frameCentimetersConstant = (Global::fieldRect.width / 176.0 + Global::fieldRect.height / 136.0) / 2;

    Global::centerAreaToDeffend.x = (Global::areaToDeffend.x + Global::areaToDeffend.x + Global::areaToDeffend.width) / 2;
    Global::centerAreaToDeffend.y = (Global::areaToDeffend.y + Global::areaToDeffend.y + Global::areaToDeffend.height) / 2;

    Global::centerAreaToAttack.x = (Global::areaToAttack.x + Global::areaToAttack.x + Global::areaToAttack.width) / 2;
    Global::centerAreaToAttack.y = (Global::areaToAttack.y + Global::areaToAttack.y + Global::areaToAttack.height) / 2;
    std::cout << "Finished calibration..." << std::endl;
    cv::setMouseCallback("Game resized", NULL, NULL);
    cv::setMouseCallback("Game original", NULL, NULL); 
   // cv::destroyWindow("Game resized");
   // cv::destroyWindow("Game original");
}

void Vision::calibrateColor(ColorDetection * color){
    cout << "Initializing calibration of " << color->getWindowName() << endl;
    color->initCalibrate();
    cout << "Creating trackbars for " << color->getWindowName() << endl;
    color->showTrackBar();
    cout << "Entering calibration of color "<< color->getWindowName() << endl;
    Global::bufferKeyboard = '0';
    while(Global::bufferKeyboard != 32)
    {
         readFrame();
         color->calibrate();
         show(color->getFilterState());
         color->detect();
         Global::bufferKeyboard = waitKey(16);
      }
      cout << "Exiting calibration of color "<< color->getWindowName() << endl;
      color->exitCalibrate();
      waitKey(16);
      this->_colorItens.insert(std::make_pair(color->getWindowName(), color));
      std::cout << "Added color for " << color->getWindowName() << std::endl;
}

void Vision::calibration()
{
    readFrame();
    // Ball
    this->calibrateColor(new ColorDetection("ball", &this->_gpuFrame));
    // Robots
    this->calibrateColor(new ColorDetection(this->config->r0.color, &this->_gpuFrame));
    this->calibrateColor(new ColorDetection(this->config->r1.color, &this->_gpuFrame));
    this->calibrateColor(new ColorDetection(this->config->r2.color, &this->_gpuFrame));
    // Team
    this->calibrateColor(new ColorDetection(this->config->teamColor, &this->_gpuFrame));
    // Adversary
    std::string  adversaryColor = this->config->teamColor == "blue" ? "yellow" : "blue";
    this->calibrateColor(new ColorDetection(adversaryColor, &this->_gpuFrame));

    cv::destroyWindow("Game original");
}

void Vision::updateRobot(const std::vector<Coordinates<double>>& colorIndividual, const std::vector<Coordinates<double>>& colorTeam, std::string role){

    if(role == "attacker")
        calculatePosRobot(colorIndividual, colorTeam, &Global::attacker);
    else if(role == "deffender")
        calculatePosRobot(colorIndividual, colorTeam, &Global::deffender);
    else
        calculatePosRobot(colorIndividual, colorTeam, &Global::goalkeeper);

}

void Vision::detectionColors()
{
   std::string adversaryColor = this->config->teamColor == "blue" ? "yellow" : "blue";
   readFrame();
    if(this->_gpuFrame.empty())
        return;
        
   std::vector<Coordinates<double>> ball = _colorItens["ball"]->getPositionsColor();
   if(ball.size() == 0)
   {
       Global::ball.set(-10, -10);
   }
   if(ball.size() > 0) {
       if(Global::ballPos.x <= -1)
       {
           Global::ballPos.set(ball[0].x, ball[0].y);
           Global::ball.set(Global::ballPos.x, Global::ballPos.y);
       }
       else
       {
           Global::lastBallPos.set(Global::ballPos.x, Global::ballPos.y);
           Global::ballPos.set(ball[0].x, ball[0].y);
           Global::ballVel.set(Global::ballPos.x - Global::lastBallPos.x, Global::ballPos.y - Global::lastBallPos.y);
           if(!(abs(Global::ballVel.x * 10) <= 3 && abs(Global::ballVel.y * 10) <= 3))
               Global::ball.set(Global::ballPos.x + (Global::ballVel.x * 10), Global::ballPos.y + (Global::ballVel.y * 10));

           if(Global::ball.x < Global::fieldRect.x)
               Global::ball.set(Global::ballPos.x, Global::ball.y);
           else if(Global::ball.x > Global::fieldRect.x + Global::fieldRect.width)
               Global::ball.set((Global::fieldRect.x + Global::fieldRect.width - Global::ball.x - Global::fieldRect.x + Global::fieldRect.width), Global::ball.y);

           if(Global::ball.y < Global::fieldRect.y)
               Global::ball.set(Global::ball.x, Global::ballPos.y);
           else if(Global::ball.y > Global::fieldRect.y + Global::fieldRect.height)
               Global::ball.set(Global::ball.x, Global::fieldRect.y + Global::fieldRect.height - (Global::fieldRect.y + Global::fieldRect.height - Global::ball.y));
       }
        if(Global::isInsideOwnArea(Global::ballPos))
            Global::ballInAreaCounter++;
        else
            Global::ballInAreaCounter = 0;
   }

   this->logger->logInformation("Ball : "+ std::to_string(Global::ball.x) + ", "+ std::to_string(Global::ball.y));

   std::vector<Coordinates<double>> teamColor = _colorItens[this->config->teamColor]->getPositionsColor();

   if(teamColor.size() > 0)
   {
         std::vector<Coordinates<double>> r0Color = _colorItens[this->config->r0.color]->getPositionsColor();
         updateRobot(r0Color, teamColor, this->config->r0.role);
         std::vector<Coordinates<double>> r1Color = _colorItens[this->config->r1.color]->getPositionsColor();
         updateRobot(r1Color, teamColor, this->config->r1.role);
         std::vector<Coordinates<double>> r2Color = _colorItens[this->config->r2.color]->getPositionsColor();
         updateRobot(r2Color, teamColor, this->config->r2.role);
   }

   std::vector<Coordinates<double >> enemyColor = _colorItens[adversaryColor]->getPositionsColor();
   Vector2D enemyPosition;
   if(teamColor.size() > 0)
   {
       for(int i=0; i < enemyColor.size(); i++)
       {
           enemyPosition.set(enemyColor[i].x, enemyColor[i].y);
           Global::enemyTeam.push_back(enemyPosition);
       }
   }

}

void Vision::show(cv::UMat frame){
    
    imshow("Game original", frame);
}

void Vision::show(bool resized)
{
   Mat newFrame;
   if(resized){
        resize(_gpuFrame, newFrame, newFrame.size(), 1.75, 1.75);
        imshow("Game resized", newFrame);
        return;
   }
   imshow("Game original", _gpuFrame);
}

void Vision::readFrame(bool cropped)
{
   _videoCapture >> _gpuFrame;
   if(cropped){
        cv::Rect2i r(Global::fieldRect.x, Global::fieldRect.y, Global::fieldRect.width,Global::fieldRect.height);
        _gpuFrame = _gpuFrame(r);
   }
}

void Vision::drawLine(const Coordinates<double> p1, const Coordinates<double> p2, const Scalar color)
{
   line(_gpuFrame, Point(p1.x, p1.y), Point(p2.x, p2.y), color, 3);
}

void Vision::calculatePosRobot(const std::vector<Coordinates<double>>& colorIndividual, const std::vector<Coordinates<double>>& colorTeam, Robot * robot)
{
   if(colorIndividual.size() == 0)
         return;

   double minDistance = 999999;
   Coordinates<double> auxIndividual = colorIndividual[0];
   Coordinates<double> auxTeam = colorTeam[0];

   for(int i = 0; i < colorIndividual.size(); i++)
   {
      for(int j = 0; j < colorTeam.size(); j++)
      {
         double dist = hypot(colorTeam[j].x - colorIndividual[i].x, colorTeam[j].y - colorIndividual[i].y);
         if(dist != 0 && dist < minDistance)
         {
            minDistance = dist;
            auxIndividual = colorIndividual[i];
            auxTeam = colorTeam[j];
         }
      }
   }

   double lineX = auxIndividual.x - auxTeam.x;
   double lineY = auxIndividual.y - auxTeam.y;

   robot->setPosition((auxIndividual.x + auxTeam.x) / 2.0, (auxIndividual.y + auxTeam.y) / 2.0);
   robot->setOrientationRobot(lineX, lineY);
    
}

void Vision::writeRectFields()
{
    std::ofstream arq;
    arq.open("fieldsSaves.txt");

    if(arq.is_open())
    {
        arq.write((char*)&Global::fieldRect, sizeof(Global::fieldRect));
        arq.write((char*)&Global::areaToAttack, sizeof(Global::areaToAttack));
        arq.write((char*)&Global::areaToDeffend, sizeof(Global::areaToDeffend));
        arq.write((char*)&Global::areaGoalAttack, sizeof(Global::areaGoalAttack));
           arq.write((char*)&Global::areaGoalDeffend, sizeof(Global::areaGoalDeffend));
    }

    arq.close();
}

void Vision::readRectFields()
{
    std::ifstream arq;
    arq.open("fieldsSaves.txt");

    if(arq.is_open())
    {
        arq.read((char*)&Global::fieldRect, sizeof(Global::fieldRect));
        arq.read((char*)&Global::areaToAttack, sizeof(Global::areaToAttack));
        arq.read((char*)&Global::areaToDeffend, sizeof(Global::areaToDeffend));
        arq.read((char*)&Global::areaGoalAttack, sizeof(Global::areaGoalAttack));
        arq.read((char*)&Global::areaGoalDeffend, sizeof(Global::areaGoalDeffend));
    }
    arq.close();
}