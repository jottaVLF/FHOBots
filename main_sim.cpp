#include <iostream>
#include "config/Config.hpp"
#include "Global.hpp"
#include "vision/FakeVision.hpp"
#include "communication/FakeCommunication.hpp"
#include "FakePhysics.hpp"
#include "model/Robot.hpp"
#include <thread>
#include <mutex>

int main() {
    Global::communication = new FakeCommunication();

    {
        std::lock_guard<std::mutex> lock(Global::worldMutex);
        Global::attacker.createMachineStates();
        Global::deffender.createMachineStates();
        Global::goalkeeper.createMachineStates();

        Global::attacker.setPosition(0, 0);
        Global::deffender.setPosition(0, 0);
        Global::goalkeeper.setPosition(0, 0);
        Global::ballPos.set(-1, -1);

        Global::communication->stopAll();
    }

    FakeVision vision;
    vision.adjustFieldPosition();
    vision.calibration();

    std::cout << "[SIM] Simulation started. Click robots/ball to reposition. ESC to quit." << std::endl;

    Global::running.store(true);
    Global::bufferKeyboard = 0;

    std::thread tAttacker(&Robot::updateRobot, &Global::attacker);
    std::thread tDeffender(&Robot::updateRobot, &Global::deffender);
    std::thread tGoalKeeper(&Robot::updateRobot, &Global::goalkeeper);

    do {
        vision.detectionColors();
        {
            std::lock_guard<std::mutex> lock(Global::worldMutex);
            Physics::updateRobots();
        }

        Global::countFrameAttacker++;
        Global::countFrameDefender++;

        vision.show();

        const int key = cv::waitKey(16); // ~60 fps
        Global::bufferKeyboard = key;
        if (key == 27)
            Global::running.store(false);

        Global::communication->sendMessage();
    } while (Global::running.load());

    Global::running.store(false);
    tAttacker.join();
    tDeffender.join();
    tGoalKeeper.join();

    Global::communication->stopAll();
    delete Global::communication;
    return 0;
}
