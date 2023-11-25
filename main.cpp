#include <iostream>
#include "config/ConfigParser.hpp"
#include "config/Config.hpp"
#include "opencv2/opencv.hpp"
#include "Global.hpp"
#include "vision/CustomTrackbar.hpp"
#include <thread>
#include <cstring>
#include "vision/Vision.hpp"
#include "vision/VisionSim.hpp"
#include "communication/CommunicationSim.hpp"
#include "model/Robot.hpp"
#include "sim/extApi.h"
#include "sim/SimDigitalTwin.hpp"
#include "input/JoystickHandle.hpp"
#include "input/JoystickSimulationInterface.hpp"
#include <unistd.h>
#include <signal.h>
int pid_son = -1;

int connect(int simPort, const char * simAddress){
    int clientID=simxStart((simxChar*)simAddress,simPort,true,true,5000,5);
	if (clientID==-1){
		printf("Could not connect to Coppelia remote API server\n");
		simxFinish(clientID);
        exit(0);
	}else{
		printf("Connected to remote API server\n");
	}

    return clientID;
}

void startSimulator(std::string simulatorPath, std::string scenePath){

    pid_son = fork();

    if(pid_son == 0){
        int fd = open("sim/simout.txt", O_CREAT | O_WRONLY, S_IWUSR);
        char * const  args[] = {(char *) simulatorPath.c_str(), (char *) scenePath.c_str(), NULL};
       if(dup2(fd, 1)!=-1)
            execv((const char *) simulatorPath.c_str(), args);
    }else{
        std::cout << "Press 'c' to continue or 'q' to quit"<< std::endl;
        char c;
        std::cin >> c;
        while(c != 'c' && c != 'q'){
            std::cout << "Press 'c' to continue or 'q' to quit"<< std::endl;
            std::cin >> c;
        }
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
}

int main(int argc, char* argv[])
{   
    ConfigParser configParser;

    Config configuration = configParser.createConfiguration();

    std::cout << "Configuration file successfully read!" << std::endl;
    
    bool isSimulation = false;
    IVision * vision;
    JoystickHandle * joystick = NULL; 
    SimDigitalTwin * digitalTwin = NULL;
    int clientId;
    if(argc > 1 && (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "-ds") == 0 || strcmp(argv[1], "-f") == 0)){              
        startSimulator(configuration.simulatorPath, configuration.simulatorScene);
        clientId = connect(configuration.simulatorPort, configuration.simulatorAddress.c_str());
        vision = strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "-f") == 0? new VisionSim(clientId, 640, 480) : new VisionSim(clientId, 640, 480, true);
        Global::communication = new CommunicationSim(clientId);
        isSimulation = true;
        Global::eAreaAttack = AREA_ATTACK_RIGHT;
        if(strcmp(argv[1], "-f") == 0){
            JoystickSimulationInterface * simulator = new JoystickSimulationInterface(clientId);
            joystick = new JoystickHandle(simulator);
            digitalTwin = new SimDigitalTwin(clientId, 640, 480);
        }    
    }else{
        vision = new Vision(configuration.camera, &configuration);
        Global::communication = new Communication(configuration.communication);
    }

    vision->adjustFieldPosition();
    vision->calibration();

    

    std::cout << "Initial configurations and calibrations ... OK";

    initializeModelAndStates(configuration);

    if(Global::hasJoystick)
        Global::attacker.setJoystickState();

    Global::bufferKeyboard = 0;
    std::thread tAttacker(&Robot::updateRobot, &Global::attacker);
    std::thread tDeffender(&Robot::updateRobot, &Global::deffender);
    std::thread tGoalKeeper(&Robot::updateRobot, &Global::goalkeeper);
    if(isSimulation){
        Global::bufferKeyboard = 32;
        sleep(2);
        Global::bufferKeyboard = 0;
    }
    do
    {
       vision->detectionColors();
       
       if(!isSimulation){
            Global::countFrameAttacker++;
            Global::countFrameDefender++;
            Global::bufferKeyboard = cv::waitKey(1);
            Vision * realVision = dynamic_cast<Vision * >(vision);
            realVision->show();
        }
        
        if(joystick != NULL)
            joystick->handleJoystickInputs();
     
        //std::this_thread::sleep_for(std::chrono::microseconds(33000));

        Global::communication->sendMessage();
        
        if(digitalTwin != NULL)
            digitalTwin->synchronize();
    } while(Global::bufferKeyboard != 27);

    tAttacker.join();
 //   tDeffender.join();
//    tGoalKeeper.join();

    Global::communication->stopAll();
    if(pid_son > 0)
        kill(pid_son, 9);
    return 0;
}