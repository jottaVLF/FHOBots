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
#include "debug/Debug.hpp"
#include "strategy/dsl/VssScript.hpp"

namespace {

bool isStrategyCheckArg(const std::string& arg)
{
    return arg == "--check-strategy" || arg == "--print-strategy";
}

}

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
    bool checkStrategy = false;
    std::string configPath = "config/appConfig.json";
    std::string strategyPath = "strategy/scripts/default.vss";

    for(int i = 1; i < argc; i++){
        std::string arg = argv[i];
        if(arg == "sim"){
            isSimulation = true;
            Global::isSim = true;
        }else if(isStrategyCheckArg(arg)){
            checkStrategy = true;
        }else if(arg.find(".vss") != std::string::npos){
            strategyPath = arg;
        }else{
            configPath = arg;
        }
    }

    if(checkStrategy){
        vssscript::VssScript strategy;
        if(!strategy.loadFromFile(strategyPath)){
            std::cerr << "DSL strategy invalid: " << strategy.error() << std::endl;
            return 1;
        }
        std::cout << strategy.describe();
        return 0;
    }

    ConfigParser configParser(configPath);

    Config configuration = configParser.createConfiguration();

    std::cout << "Configuration file successfully read: " << configPath << std::endl;
    
    IVision * vision = isSimulation ? (IVision *) new SimVision(&configuration, "224.0.0.1", 10002) : (IVision *) new Vision(configuration.camera, &configuration);
    Global::communication = isSimulation ? (ICommunication *) new SimCommunication(&configuration, "127.0.0.1", 20011) : (ICommunication *) new Communication(configuration.communication);
     
    std::cout << "Vision initialized " <<std::endl;
    initializeModelAndStates(configuration);

    vssscript::VssScript strategy;
    if(strategy.loadFromFile(strategyPath)){
        std::cout << "DSL strategy loaded: " << strategy.name() << " (" << strategyPath << ")" << std::endl;
    }else{
        std::cout << "DSL strategy disabled: " << strategy.error() << std::endl;
    }

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
        {
            std::lock_guard<std::recursive_mutex> lock(Global::worldMutex);
            vision->detectionColors();
            strategy.execute();
        }
        
        Global::countFrameAttacker++;
        Global::countFrameDefender++;
        if(isSimulation){
            if(debug == nullptr)
                debug = new Debug(configuration.teamColor == "yellow");
            std::lock_guard<std::recursive_mutex> lock(Global::worldMutex);
            debug->show();
        }else{
            Vision * realVision = dynamic_cast<Vision * >(vision);
            std::lock_guard<std::recursive_mutex> lock(Global::worldMutex);
            realVision->show();
        }
        Global::bufferKeyboard = cv::waitKey(1);
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
