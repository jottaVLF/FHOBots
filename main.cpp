#include <iostream>
#include "config/ConfigParser.hpp"
#include "config/Config.hpp"
#include "opencv2/opencv.hpp"
#include "Global.hpp"
#include "vision/CustomTrackbar.hpp"
#include <thread>
#include <cstring>
#include <mutex>
#include "vision/Vision.hpp"
#include "vision/FakeVision.hpp"
#include "communication/CommunicationESPNow.hpp"
#include "communication/CommunicationWIFICalibration.hpp"
#include "model/Robot.hpp"
#include "model/Robot.hpp"
#include "FakePhysics.hpp"

void initializeModelAndStates(Config configuration){
    std::lock_guard<std::mutex> lock(Global::worldMutex);

    Global::attacker.createMachineStates();
    Global::deffender.createMachineStates();
    Global::goalkeeper.createMachineStates();

    Global::attacker.setPosition(0, 0);
    Global::deffender.setPosition(0, 0);
    Global::goalkeeper.setPosition(0, 0);

    Global::ballPos.set(-1, -1);

    Global::goalkeeper.setPD(configuration.r0.control.frontLeft.kp, configuration.r0.control.frontLeft.kd);
    Global::deffender.setPD(configuration.r1.control.frontLeft.kp, configuration.r1.control.frontLeft.kd);
    Global::attacker.setPD(configuration.r2.control.frontLeft.kp, configuration.r2.control.frontLeft.kd);

    Global::communication->stopAll();
}

int main(int argc, char* argv[])
{ 
    ConfigParser configParser;

    Config configuration = configParser.createConfiguration();

    std::cout << "Configuration file successfully read!" << std::endl;
    
    IVision * vision = new Vision(configuration.camera, &configuration);
    Global::communication = new CommunicationESPNow(configuration.communication);
    CommunicationWIFICalibration server("192.168.0.108", 8888);
    std::thread serverLidten(&CommunicationWIFICalibration::receiveMessage, & server);
    
    initializeModelAndStates(configuration);

    vision->calibratePreProcess();
    vision->adjustFieldPosition();
    vision->calibration(); 

    std::cout << "Initial configurations and calibrations ... OK" <<std::endl;

    Global::running.store(true);
    Global::bufferKeyboard = 0;
    std::thread tAttacker(&Robot::updateRobot, &Global::attacker);
    std::thread tDeffender(&Robot::updateRobot, &Global::deffender);
    std::thread tGoalKeeper(&Robot::updateRobot, &Global::goalkeeper);
    int i = 0;
    do
    {
        //std::cout << "Dectect vision ... " <<std::endl;
        vision->detectionColors();
        
        Global::countFrameAttacker++;
        Global::countFrameDefender++;
        const int key = cv::waitKey(1);
        Global::bufferKeyboard = key;
        if(key == 27)
            Global::running.store(false);
        Vision * realVision = dynamic_cast<Vision * >(vision);
        //std::cout << "Vision show ... " <<std::endl;
        realVision->show();
        //std::this_thread::sleep_for(std::chrono::microseconds(500));
        //std::cout << "Send Message ... " <<std::endl;
        Global::communication->sendMessage();
        //Global::communication->getMessage();
    } while(Global::running.load());

    Global::running.store(false);
    tAttacker.join();
    tDeffender.join();
    tGoalKeeper.join();

    Global::communication->stopAll();
    Global::communication->sendMessage();
    serverLidten.detach();
    return 0;
}
