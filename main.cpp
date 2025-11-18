#include <iostream>
#include "config/ConfigParser.hpp"
#include "opencv2/opencv.hpp"
#include "Global.hpp"
#include "vision/CustomTrackbar.hpp"
#include <thread>
#include <cstring>
#include "vision/Vision.hpp"
#include "vision/SimVision.hpp"
#include "communication/SimCommunication.hpp"
#include "model/Robot.hpp"
#include "model/Robot.hpp"
#include "debug/Debug.hpp"

void initializeModelAndStates(Config configuration){
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
    std::cout << "Model and states initialized" << std::endl;
}

int main(int argc, char* argv[])
{ 
    Debug * debug = nullptr;
    bool isSimulation = false;
    
    if(argc > 1 && strcmp(argv[1], "sim") == 0){
        isSimulation = true;
    }

    ConfigParser configParser;

    Config configuration = configParser.createConfiguration();

    std::cout << "Configuration file successfully read!" << std::endl;
    
    IVision * vision =  isSimulation ? (IVision *) new SimVision(&configuration, "224.0.0.1", 10002) : (IVision *) new Vision(configuration.camera, &configuration);
    Global::communication = isSimulation ? (ICommunication * ) new SimCommunication(&configuration, "127.0.0.1", 20011, 60) : (ICommunication * ) new Communication(configuration.communication);
     
    std::cout << "Vision initialized " <<std::endl;
    initializeModelAndStates(configuration);

    vision->adjustFieldPosition();
    vision->calibration(); 

    std::cout << "Initial configurations and calibrations ... OK" <<std::endl;

    Global::bufferKeyboard = 0;
    std::thread tAttacker(&Robot::updateRobot, &Global::attacker);
    std::thread tDeffender(&Robot::updateRobot, &Global::deffender);
    std::thread tGoalKeeper(&Robot::updateRobot, &Global::goalkeeper);
    int i = 0;
    do
    {
        vision->detectionColors();
        
        Global::countFrameAttacker++;
        Global::countFrameDefender++;
        Global::bufferKeyboard = cv::waitKey(1);
        if(isSimulation){
            if(debug == nullptr)
                debug = new Debug(configuration.teamColor == "yellow");
            debug->show();
        }else{
            Vision * realVision = dynamic_cast<Vision * >(vision);
            realVision->show();
        }
        std::this_thread::sleep_for(std::chrono::microseconds(500));
        Global::communication->sendMessage();
    } while(Global::bufferKeyboard != 27);

    tAttacker.join();
    tDeffender.join();
    tGoalKeeper.join();

    Global::communication->stopAll();
    Global::communication->sendMessage();
 
    return 0;
}