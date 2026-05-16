#include "Vision.hpp"
#include "../model/Field.hpp"
#include <sstream>
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mutex>
#include <limits>
using namespace std;
using namespace cv;

namespace
{
    constexpr int TRACKED_ROBOT_COUNT = 3;
    constexpr double MAX_TRACKING_JUMP_PIXELS = 90.0;
    constexpr int MAX_LOST_FRAMES_BEFORE_REACQUIRE = 12;
    constexpr double MARKER_PAIR_TIE_WEIGHT = 0.05;
    constexpr double ROBOT_KALMAN_PROCESS_NOISE = 5e-2;
    constexpr double ROBOT_KALMAN_MEASUREMENT_NOISE = 9.0;
    constexpr double ROBOT_KALMAN_ERROR_COVARIANCE = 25.0;
    constexpr double ROBOT_KALMAN_DT = 1.0;
    constexpr double ROBOT_HOLD_VELOCITY_DECAY = 0.85;
    constexpr double BALL_KALMAN_PROCESS_NOISE = 1e-2;
    constexpr double BALL_KALMAN_MEASUREMENT_NOISE = 4.0;
    constexpr double BALL_KALMAN_ERROR_COVARIANCE = 10.0;
    constexpr double BALL_KALMAN_DT = 1.0;
    constexpr double BALL_KALMAN_GATING_DISTANCE_PIXELS = 60.0;
    constexpr int MAX_BALL_LOST_FRAMES = 10;
    constexpr double BALL_PREDICTION_HORIZON_FRAMES = 10.0;
    constexpr double DEBUG_VELOCITY_DRAW_SCALE = 6.0;
    constexpr double DEBUG_VELOCITY_MAX_LENGTH_PIXELS = 45.0;
    constexpr double DEBUG_VELOCITY_MIN_LENGTH_PIXELS = 1.0;

    double distanceBetween(double x1, double y1, double x2, double y2)
    {
        return hypot(x1 - x2, y1 - y2);
    }

    bool isFiniteVector(const Vector2D& value)
    {
        return std::isfinite(value.x) && std::isfinite(value.y);
    }

    bool selectClosestMeasurement(const std::vector<Coordinates<double>>& detections,
                                  const Vector2D& reference,
                                  Coordinates<double>& selected,
                                  double& selectedDistance)
    {
        bool found = false;
        selectedDistance = std::numeric_limits<double>::max();

        for(std::size_t i = 0; i < detections.size(); i++)
        {
            if(!std::isfinite(detections[i].x) || !std::isfinite(detections[i].y))
                continue;

            const double distance = distanceBetween(detections[i].x,
                                                    detections[i].y,
                                                    reference.x,
                                                    reference.y);
            if(distance < selectedDistance)
            {
                selected = detections[i];
                selectedDistance = distance;
                found = true;
            }
        }

        return found;
    }

    Vector2D lostBallFallbackPosition()
    {
        if(Global::fieldRect.width > 0 && Global::fieldRect.height > 0)
        {
            return Vector2D(Global::fieldRect.x + Global::fieldRect.width / 2.0,
                            Global::fieldRect.y + Global::fieldRect.height / 2.0);
        }

        return Vector2D(0, 0);
    }
}

Vision::Vision(int deviceNumber, Config * config)
{
    int initial[] = {225, 375, 250, 250};
    for(int i = 0; i < 4; i++)
        _cameraCoeficients[i] = initial[i];

    for(int i = 0; i < TRACKED_ROBOT_COUNT; i++)
    {
        _lastTrackedPosition[i].set(0, 0);
        _hasTrackedPosition[i] = false;
        _trackingHeld[i] = false;
        _lostFrames[i] = 0;
        _robotKalman[i].configure(ROBOT_KALMAN_PROCESS_NOISE,
                                  ROBOT_KALMAN_MEASUREMENT_NOISE,
                                  ROBOT_KALMAN_ERROR_COVARIANCE,
                                  ROBOT_KALMAN_DT);
        _lastRobotMeasurement[i].set(-1, -1);
        _lastRobotFiltered[i].set(-1, -1);
        _hasRobotMeasurement[i] = false;
        _hasRobotFilter[i] = false;
    }
    _ballKalman.configure(BALL_KALMAN_PROCESS_NOISE,
                          BALL_KALMAN_MEASUREMENT_NOISE,
                          BALL_KALMAN_ERROR_COVARIANCE,
                          BALL_KALMAN_DT);
    _lastBallMeasurement.set(-1, -1);
    _lastBallFiltered.set(-1, -1);
    _lastBallPrediction.set(-10, -10);
    _hasBallMeasurement = false;
    _hasBallFilter = false;
    _ballPredictionOnly = false;
    _ballLostFrames = 0;

    bool success = _videoCapture.open(deviceNumber);
    if(!success){
        std::cerr << "Error: could not open camera device " << deviceNumber << ". Check camera connection and config/appConfig.json." << std::endl;
        exit(1);
    }
    if(!ocl::useOpenCL())
       ocl::setUseOpenCL(true);

    _videoCapture.set(3, 640);
    _videoCapture.set(4, 480);
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
            f[i].draw(_gpuFrame);
            putText(_gpuFrame, f[i].getName(), Point(30, 50), 0, 1.5, Scalar(0, 0, 255), 5);
            f[i].setWindow();
            show(false);
            Global::bufferKeyboard = waitKey(16);
        }
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
    }

    writeRectFields();
    Global::frameCentimetersConstant = (Global::fieldRect.width / 176.0 + Global::fieldRect.height / 136.0) / 2;

    Global::centerAreaToDeffend.x = (Global::areaToDeffend.x + Global::areaToDeffend.x + Global::areaToDeffend.width) / 2;
    Global::centerAreaToDeffend.y = (Global::areaToDeffend.y + Global::areaToDeffend.y + Global::areaToDeffend.height) / 2;

    Global::centerAreaToAttack.x = (Global::areaToAttack.x + Global::areaToAttack.x + Global::areaToAttack.width) / 2;
    Global::centerAreaToAttack.y = (Global::areaToAttack.y + Global::areaToAttack.y + Global::areaToAttack.height) / 2;
    std::cout << "Finished calibration..." << std::endl;
    cv::setMouseCallback("Game original", NULL, NULL); 
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
         color->detect(true);
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
   {
      std::lock_guard<std::mutex> lock(Global::worldMutex);
      Global::enemyTeam.clear();
      updateBallKalman(std::vector<Coordinates<double>>());
      markRobotDetectionLost(&Global::attacker);
      markRobotDetectionLost(&Global::deffender);
      markRobotDetectionLost(&Global::goalkeeper);
      return;
   }

   std::vector<Coordinates<double>> ball = _colorItens["ball"]->getPositionsColor();
   std::vector<Coordinates<double>> teamColor = _colorItens[this->config->teamColor]->getPositionsColor();
   std::vector<Coordinates<double>> r0Color;
   std::vector<Coordinates<double>> r1Color;
   std::vector<Coordinates<double>> r2Color;

   if(teamColor.size() > 0)
   {
         r0Color = _colorItens[this->config->r0.color]->getPositionsColor();
         r1Color = _colorItens[this->config->r1.color]->getPositionsColor();
         r2Color = _colorItens[this->config->r2.color]->getPositionsColor();
   }

   std::vector<Coordinates<double >> enemyColor = _colorItens[adversaryColor]->getPositionsColor();
   double loggedBallX = 0;
   double loggedBallY = 0;
   {
      std::lock_guard<std::mutex> lock(Global::worldMutex);

      Global::enemyTeam.clear();

      updateBallKalman(ball);

      if(teamColor.size() > 0)
      {
            updateRobot(r0Color, teamColor, this->config->r0.role);
            updateRobot(r1Color, teamColor, this->config->r1.role);
            updateRobot(r2Color, teamColor, this->config->r2.role);
      }
      else
      {
            markRobotDetectionLost(&Global::attacker);
            markRobotDetectionLost(&Global::deffender);
            markRobotDetectionLost(&Global::goalkeeper);
      }

      Vector2D enemyPosition;
      if(teamColor.size() > 0)
       {
          for(int i=0; i < enemyColor.size(); i++)
          {
             enemyPosition.set(enemyColor[i].x, enemyColor[i].y);
             Global::enemyTeam.push_back(enemyPosition);
          }
       }

      loggedBallX = Global::ball.x;
      loggedBallY = Global::ball.y;
   }

   this->logger->logInformation("Ball : "+ std::to_string(loggedBallX) + ", "+ std::to_string(loggedBallY));
}

void Vision::show(cv::UMat frame){
    cv::Mat output;
    std::vector<UMat> channels(3);
    std::vector<UMat> outPuts(3);
    cv::split(_gpuFrame, channels);
    for(int i = 0; i < channels.size(); i++){
        cv::bitwise_and(channels[i],frame,outPuts[i]);
    }

    cv::merge(outPuts, output);
    imshow("Game original", output);
}

void Vision::show(bool resized)
{
   Mat newFrame;
   if(resized){
        drawInfo();
        resize(_gpuFrame, newFrame, newFrame.size(), 1.25, 1.25);
        imshow("Game resized", newFrame);
        return;
   }
   imshow("Game original", _gpuFrame);
}

void Vision::readFrame(bool cropped)
{
   _videoCapture >> _gpuFrame;
   calibratePreProcessVideo();
   if(cropped){
        cv::Rect2i r(Global::fieldRect.x, Global::fieldRect.y, Global::fieldRect.width,Global::fieldRect.height);
        _gpuFrame = _gpuFrame(r);
   }
}

void Vision::calibratePreProcessVideo(){
    //cv::imshow("Original", _gpuFrame);
    Mat cameraMatrix = (Mat_<double>(3, 3) << 800, 0, 320, 0, 800, 240, 0, 0, 1);  
    Mat distCoeffs = (Mat_<double>(4, 1) << 2*_cameraCoeficients[0]/500. - 1, 
                                            2*_cameraCoeficients[1]/500. - 1, 
                                            2*_cameraCoeficients[2]/500. - 1, 
                                            2*_cameraCoeficients[3]/500. - 1);
    
    //cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    //clahe->setClipLimit(2.0); // valor comum é 2.0
    //clahe->setTilesGridSize(cv::Size(8, 8)); 

    Mat undistorted;  
  //  Mat gray, clahe_result;

    if(_gpuFrame.empty())
        return;
    //cvtColor(_gpuFrame, gray, COLOR_BGR2GRAY);
    //clahe->apply(gray, clahe_result);
    //cvtColor(clahe_result, _gpuFrame, COLOR_GRAY2BGR);
    
    undistort(_gpuFrame, undistorted, cameraMatrix, distCoeffs);
    undistorted.copyTo(_gpuFrame);    
    

      
}

void Vision::drawLine(const Coordinates<double> p1, const Coordinates<double> p2, const Scalar color)
{
   line(_gpuFrame, Point(p1.x, p1.y), Point(p2.x, p2.y), color, 3);
}

Vector2D Vision::keepBallPredictionInsideField(Vector2D predicted, Vector2D filteredPosition)
{
   const double minX = Global::fieldRect.x;
   const double maxX = Global::fieldRect.x + Global::fieldRect.width;
   const double minY = Global::fieldRect.y;
   const double maxY = Global::fieldRect.y + Global::fieldRect.height;

   if(predicted.x < minX || predicted.x > maxX)
      predicted.x = filteredPosition.x;
   if(predicted.y < minY || predicted.y > maxY)
      predicted.y = filteredPosition.y;

   return predicted;
}

void Vision::updateBallKalman(const std::vector<Coordinates<double>>& ballDetections)
{
   _hasBallMeasurement = false;

   if(ballDetections.size() > 0)
   {
      if(!_ballKalman.isInitialized())
      {
         bool hasInitialMeasurement = false;
         Coordinates<double> initialMeasurement = ballDetections[0];

         for(std::size_t i = 0; i < ballDetections.size(); i++)
         {
            if(std::isfinite(ballDetections[i].x) && std::isfinite(ballDetections[i].y))
            {
               initialMeasurement = ballDetections[i];
               hasInitialMeasurement = true;
               break;
            }
         }

         if(hasInitialMeasurement)
         {
            _lastBallMeasurement.set(initialMeasurement.x, initialMeasurement.y);
            _ballKalman.init(initialMeasurement.x, initialMeasurement.y);
            _hasBallMeasurement = true;
            _ballLostFrames = 0;
         }
         else
         {
            _ballLostFrames++;
         }
      }
      else
      {
         _ballKalman.predict();
         Vector2D predictedForGate = _ballKalman.position();
         Coordinates<double> selectedMeasurement = ballDetections[0];
         double gateDistance = 0.0;

         if(selectClosestMeasurement(ballDetections,
                                     predictedForGate,
                                     selectedMeasurement,
                                     gateDistance) &&
            gateDistance <= BALL_KALMAN_GATING_DISTANCE_PIXELS)
         {
            _lastBallMeasurement.set(selectedMeasurement.x, selectedMeasurement.y);
            _ballKalman.correct(selectedMeasurement.x, selectedMeasurement.y);
            _hasBallMeasurement = true;
            _ballLostFrames = 0;
         }
         else
         {
            _ballLostFrames++;
         }
      }
   }
   else
   {
      _ballLostFrames++;

      if(!_ballKalman.isInitialized() || _ballLostFrames > MAX_BALL_LOST_FRAMES)
      {
         _ballKalman.reset();
         _hasBallFilter = false;
         _ballPredictionOnly = false;
         Vector2D fallback = lostBallFallbackPosition();
         Global::lastBallPos.set(Global::ballPos.x, Global::ballPos.y);
         Global::ballPos.set(-1, -1);
         Global::ballVel.set(0, 0);
         Global::ball.set(fallback.x, fallback.y);
         Global::ballInAreaCounter = 0;
         _lastBallFiltered.set(-1, -1);
         _lastBallPrediction.set(fallback.x, fallback.y);
         return;
      }

      if(ballDetections.size() == 0)
         _ballKalman.predict();
   }

   if(!_hasBallMeasurement && (!_ballKalman.isInitialized() || _ballLostFrames > MAX_BALL_LOST_FRAMES))
   {
      _ballKalman.reset();
      _hasBallFilter = false;
      _ballPredictionOnly = false;
      Vector2D fallback = lostBallFallbackPosition();
      Global::lastBallPos.set(Global::ballPos.x, Global::ballPos.y);
      Global::ballPos.set(-1, -1);
      Global::ballVel.set(0, 0);
      Global::ball.set(fallback.x, fallback.y);
      Global::ballInAreaCounter = 0;
      _lastBallFiltered.set(-1, -1);
      _lastBallPrediction.set(fallback.x, fallback.y);
      return;
   }

   Vector2D filteredPosition = _ballKalman.position();
   Vector2D estimatedVelocity = _ballKalman.velocity();
   Vector2D predictedPosition = _ballKalman.predictedPosition(BALL_PREDICTION_HORIZON_FRAMES);
   predictedPosition = keepBallPredictionInsideField(predictedPosition, filteredPosition);

   Global::lastBallPos.set(Global::ballPos.x, Global::ballPos.y);
   Global::ballPos.set(filteredPosition.x, filteredPosition.y);
   Global::ballVel.set(estimatedVelocity.x, estimatedVelocity.y);
   Global::ball.set(predictedPosition.x, predictedPosition.y);

   if(Global::isInsideOwnArea(Global::ballPos))
      Global::ballInAreaCounter++;
   else
      Global::ballInAreaCounter = 0;

   _lastBallFiltered.set(filteredPosition.x, filteredPosition.y);
   _lastBallPrediction.set(predictedPosition.x, predictedPosition.y);
   _hasBallFilter = true;
   _ballPredictionOnly = !_hasBallMeasurement;
}

void Vision::calculatePosRobot(const std::vector<Coordinates<double>>& colorIndividual, const std::vector<Coordinates<double>>& colorTeam, Robot * robot)
{
   const int robotId = robot->getPosMessage();
   if(robotId < 0 || robotId >= TRACKED_ROBOT_COUNT)
      return;

   if(colorIndividual.size() == 0 || colorTeam.size() == 0)
   {
      markRobotDetectionLost(robot);
      return;
   }

   const bool useTemporalReference = _hasTrackedPosition[robotId] &&
                                     _lostFrames[robotId] <= MAX_LOST_FRAMES_BEFORE_REACQUIRE;
   double bestScore = std::numeric_limits<double>::max();
   double bestTemporalDistance = std::numeric_limits<double>::max();
   Coordinates<double> bestIndividual = colorIndividual[0];
   Coordinates<double> bestTeam = colorTeam[0];
   bool foundCandidate = false;

   for(int i = 0; i < colorIndividual.size(); i++)
   {
      for(int j = 0; j < colorTeam.size(); j++)
      {
         const double markerDistance = distanceBetween(colorTeam[j].x,
                                                       colorTeam[j].y,
                                                       colorIndividual[i].x,
                                                       colorIndividual[i].y);
         if(markerDistance == 0)
            continue;

         const double centerX = (colorIndividual[i].x + colorTeam[j].x) / 2.0;
         const double centerY = (colorIndividual[i].y + colorTeam[j].y) / 2.0;
         const double temporalDistance = useTemporalReference
            ? distanceBetween(centerX, centerY, _lastTrackedPosition[robotId].x, _lastTrackedPosition[robotId].y)
            : 0.0;
         const double score = useTemporalReference
            ? temporalDistance + markerDistance * MARKER_PAIR_TIE_WEIGHT
            : markerDistance;

         if(score < bestScore)
         {
            bestScore = score;
            bestTemporalDistance = temporalDistance;
            bestIndividual = colorIndividual[i];
            bestTeam = colorTeam[j];
            foundCandidate = true;
         }
      }
   }

   if(!foundCandidate)
   {
      markRobotDetectionLost(robot);
      return;
   }

   if(useTemporalReference && bestTemporalDistance > MAX_TRACKING_JUMP_PIXELS)
   {
      markRobotDetectionLost(robot);
      return;
   }

   const double centerX = (bestIndividual.x + bestTeam.x) / 2.0;
   const double centerY = (bestIndividual.y + bestTeam.y) / 2.0;
   const double lineX = bestIndividual.x - bestTeam.x;
   const double lineY = bestIndividual.y - bestTeam.y;

   updateRobotKalman(robot, centerX, centerY);
   robot->setOrientationRobot(lineX, lineY);

   _lastTrackedPosition[robotId].set(centerX, centerY);
   _hasTrackedPosition[robotId] = true;
   _trackingHeld[robotId] = false;
   _lostFrames[robotId] = 0;
    
}

void Vision::updateRobotKalman(Robot * robot, double measuredX, double measuredY)
{
   const int robotId = robot->getPosMessage();
   if(robotId < 0 || robotId >= TRACKED_ROBOT_COUNT)
      return;

   _hasRobotMeasurement[robotId] = true;
   _lastRobotMeasurement[robotId].set(measuredX, measuredY);

   if(!_robotKalman[robotId].isInitialized())
      _robotKalman[robotId].init(measuredX, measuredY);
   else
   {
      _robotKalman[robotId].predict();
      _robotKalman[robotId].correct(measuredX, measuredY);
   }

   Vector2D filteredPosition = _robotKalman[robotId].position();
   Vector2D estimatedVelocity = _robotKalman[robotId].velocity();

   robot->setLastPosition(robot->getPosition().x, robot->getPosition().y);
   robot->setPosition(filteredPosition);
   robot->setVelocity(estimatedVelocity);

   _lastRobotFiltered[robotId].set(filteredPosition.x, filteredPosition.y);
   _hasRobotFilter[robotId] = true;
}

void Vision::markRobotDetectionLost(Robot * robot)
{
   const int robotId = robot->getPosMessage();
   if(robotId < 0 || robotId >= TRACKED_ROBOT_COUNT)
      return;

   if(_lostFrames[robotId] < MAX_LOST_FRAMES_BEFORE_REACQUIRE + 1)
      _lostFrames[robotId]++;

   _hasRobotMeasurement[robotId] = false;
   _trackingHeld[robotId] = _hasTrackedPosition[robotId] &&
                            _lostFrames[robotId] <= MAX_LOST_FRAMES_BEFORE_REACQUIRE;

   if(_trackingHeld[robotId] && _robotKalman[robotId].isInitialized())
   {
      _robotKalman[robotId].dampVelocity(ROBOT_HOLD_VELOCITY_DECAY);
      _robotKalman[robotId].predict();
      Vector2D filteredPosition = _robotKalman[robotId].position();
      Vector2D estimatedVelocity = _robotKalman[robotId].velocity();

      robot->setLastPosition(robot->getPosition().x, robot->getPosition().y);
      robot->setPosition(filteredPosition);
      robot->setVelocity(estimatedVelocity);

      _lastRobotFiltered[robotId].set(filteredPosition.x, filteredPosition.y);
      _hasRobotFilter[robotId] = true;
   }
   else if(_lostFrames[robotId] > MAX_LOST_FRAMES_BEFORE_REACQUIRE)
   {
      _robotKalman[robotId].reset();
      _hasRobotFilter[robotId] = false;
      robot->setVelocity(Vector2D(0, 0));
   }
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

void Vision::drawInfo(){
    std::lock_guard<std::mutex> lock(Global::worldMutex);

    if(this->config->r0.active)
        drawRole(this->config->r0.role, this->config->r0.color);
    if(this->config->r1.active)
        drawRole(this->config->r1.role, this->config->r1.color);
    if(this->config->r2.active)
        drawRole(this->config->r2.role, this->config->r2.color);
    drawSafeZone();
    drawBallKalmanDebug();
}

cv::Point Vision::toPixel(Vector2D v){
    return cv::Point(v.x, v.y);
}

void Vision::drawVelocityVector(Vector2D origin, Vector2D velocity, cv::Scalar color)
{
    if(!isFiniteVector(origin) || !isFiniteVector(velocity))
        return;

    double dx = velocity.x * DEBUG_VELOCITY_DRAW_SCALE;
    double dy = velocity.y * DEBUG_VELOCITY_DRAW_SCALE;
    const double length = std::hypot(dx, dy);

    if(!std::isfinite(length) || length < DEBUG_VELOCITY_MIN_LENGTH_PIXELS)
        return;

    if(length > DEBUG_VELOCITY_MAX_LENGTH_PIXELS)
    {
        const double scale = DEBUG_VELOCITY_MAX_LENGTH_PIXELS / length;
        dx *= scale;
        dy *= scale;
    }

    cv::line(_gpuFrame,
             toPixel(origin),
             cv::Point(origin.x + dx, origin.y + dy),
             color,
             1,
             cv::LINE_4);
}

void Vision::drawRole(std::string role, std::string colorString){
    Robot * robot = NULL;
    int id = 0;
    cv::Scalar color;
    if(colorString == "purple")
        color = cv::Scalar(255, 70, 168, 0.8);
    else if(colorString == "green")
        color = cv::Scalar(0, 170, 0, 0.8);
    else if(colorString == "red")
        color = cv::Scalar(0, 0, 168, 0.8);

    if(role == "attacker"){
        robot = &Global::attacker;
        id = robot->getPosMessage();
    }
    else if(role == "deffender"){
        robot = &Global::deffender;
        id = robot->getPosMessage();
    }
    else if(role == "goalkeeper"){
        robot = &Global::goalkeeper;
        id = robot->getPosMessage();
    } 
    char robotPosition[100], controlParams[100];
    sprintf(robotPosition, "%.1lf, %.1lf", robot->getPosition().x, robot->getPosition().y);
    cv::line(_gpuFrame, toPixel(robot->getPosition()), cv::Point(robot->getPosition().x + 3* robot->getOrientation().x, robot->getPosition().y + 3* robot->getOrientation().y), cv::Scalar(255,255, 255, 1), 1, cv::LINE_4);
    drawVelocityVector(robot->getPosition(), robot->getVelocity(), cv::Scalar(0, 255, 255, 1));
    cv::putText(_gpuFrame, robot->getMessage(),cv::Point(robot->getPosition().x - 15 , robot->getPosition().y + 40) ,cv::FONT_HERSHEY_COMPLEX, 0.3, cv::Scalar(255,255,255, 1), 1);
    cv::putText(_gpuFrame, robotPosition,cv::Point(robot->getPosition().x - 15, robot->getPosition().y + 25) ,cv::FONT_HERSHEY_COMPLEX, 0.3, cv::Scalar(255,255,255, 1), 1);
    cv::line(_gpuFrame, toPixel(robot->getPosition()),  toPixel(robot->objPos), color, 1, cv::LINE_4);
    
    sprintf(robotPosition, "%d, %d", Global::communication->getLeftPwm(id), Global::communication->getRightPwm(id));
    cv::putText(_gpuFrame, robotPosition,cv::Point(robot->getPosition().x, robot->getPosition().y - 25) ,cv::FONT_HERSHEY_COMPLEX, 0.3, cv::Scalar(255,255,255, 1), 1);
    cv::circle(_gpuFrame, toPixel(robot->objPos), WorldModel::_precision, color, 1);

    if(id >= 0 && id < TRACKED_ROBOT_COUNT && _hasRobotMeasurement[id])
        cv::circle(_gpuFrame, toPixel(_lastRobotMeasurement[id]), 4, cv::Scalar(0, 255, 0, 1), 1);

    if(id >= 0 && id < TRACKED_ROBOT_COUNT && _hasRobotFilter[id])
        cv::circle(_gpuFrame, toPixel(_lastRobotFiltered[id]), 6, cv::Scalar(255, 255, 0, 1), 1);

    if(id >= 0 && id < TRACKED_ROBOT_COUNT && _trackingHeld[id])
    {
        cv::circle(_gpuFrame, toPixel(robot->getPosition()), 12, cv::Scalar(0, 180, 255, 1), 1);
        cv::putText(_gpuFrame, "hold", cv::Point(robot->getPosition().x + 10, robot->getPosition().y - 10), cv::FONT_HERSHEY_COMPLEX, 0.3, cv::Scalar(0, 180, 255, 1), 1);
    }
    
    sprintf(robotPosition, "%lf, %lf", robot->getError(), robot->getPD());
    cv::putText(_gpuFrame, robotPosition,cv::Point(robot->getPosition().x, robot->getPosition().y - 50),cv::FONT_HERSHEY_COMPLEX, 0.3, cv::Scalar(255,255,255, 1), 1);
    
}

void Vision::drawSafeZone(){
    int offset = WorldModel::_offset;
    double fieldWidth = Global::fieldRect.width;
    double fieldHeight = Global::fieldRect.height;

    double xmin =  Global::areaGoalDeffend.isOnLeft(fieldWidth/2) ? 
                   Global::areaGoalDeffend.width + offset
                  :Global::areaGoalAttack.width + offset;
    double xmax = Global::areaGoalDeffend.isOnLeft(fieldWidth/2) ? 
                   fieldWidth - (Global::areaGoalDeffend.width + offset)
                  :fieldWidth - (Global::areaGoalAttack.width + offset);
    
    double ymin = offset;
    double ymax = fieldHeight - offset;
    cv::line(_gpuFrame, cv::Point(xmin, ymin), cv::Point(xmax, ymin), cv::Scalar(20,150, 150, 1), 1, cv::LINE_4);
    cv::line(_gpuFrame, cv::Point(xmin, ymin), cv::Point(xmin, ymax), cv::Scalar(20,150, 150, 1), 1, cv::LINE_4);
    cv::line(_gpuFrame, cv::Point(xmax, ymin), cv::Point(xmax, ymax), cv::Scalar(20,150, 150, 1), 1, cv::LINE_4);
    cv::line(_gpuFrame, cv::Point(xmax, ymax), cv::Point(xmin, ymax), cv::Scalar(20,150, 150, 1), 1, cv::LINE_4);


}

void Vision::drawBallKalmanDebug(){
    if(_hasBallMeasurement)
        cv::circle(_gpuFrame, toPixel(_lastBallMeasurement), 4, cv::Scalar(0, 255, 0, 1), 1);

    if(_hasBallFilter)
    {
        cv::circle(_gpuFrame, toPixel(_lastBallFiltered), 5, cv::Scalar(255, 255, 0, 1), 1);
        cv::circle(_gpuFrame, toPixel(_lastBallPrediction), 7, cv::Scalar(0, 180, 255, 1), 1);
        drawVelocityVector(_lastBallFiltered, Global::ballVel, cv::Scalar(255, 0, 255, 1));

        if(_ballPredictionOnly)
            cv::putText(_gpuFrame, "ball hold", cv::Point(_lastBallPrediction.x + 8, _lastBallPrediction.y - 8), cv::FONT_HERSHEY_COMPLEX, 0.3, cv::Scalar(0, 180, 255, 1), 1);
    }
}

void Vision::writeCoeficients()
{
    std::ofstream arq;
    arq.open("fieldsCoeficients.txt");

    if(arq.is_open())
    {   
        for(int i = 0; i<4; i++){
            arq.write((char*)&_cameraCoeficients[i], sizeof(_cameraCoeficients[i]));
        };
        
    }else{
        std::cerr << "Error: could not open fieldsCoeficients.txt for writing camera coefficients." << std::endl;
    }

    arq.close();
}
void Vision::readCoeficients()
{
    std::ifstream arq;
    arq.open("fieldsCoeficients.txt");

    if(arq.is_open())
    {
        for(int i = 0; i<4;i++){
            arq.read((char*)&_cameraCoeficients[i], sizeof(_cameraCoeficients[i]));
        };
    }else{
        std::cerr << "Warning: could not open fieldsCoeficients.txt. Using default camera coefficients." << std::endl;
    }

    arq.close();
}
void Vision::calibratePreProcess(){
    readCoeficients();
    namedWindow("Coeficiente olho de peixe");
    for(int i = 0; i < 4; i++){
        _cameraCoeficients[i] /*= initial[i]*/;
        if(i < 2)
            cv::createTrackbar("Coeficiente " + to_string(i), "Coeficiente olho de peixe", &_cameraCoeficients[i], 500);
    }

    Global::bufferKeyboard = '0';
    while(Global::bufferKeyboard != 32){
        readFrame(false);
        show(false);
        Global::bufferKeyboard = waitKey(16);
        

   
    }
    writeCoeficients();
    cv::destroyWindow("Coeficiente olho de peixe");
}

