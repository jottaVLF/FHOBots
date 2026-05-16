#include "AttackerStateAttacking.hpp"
#include "AttackerUnivector.hpp"

#include "../../Global.hpp"
#include "../../communication/json.hpp"
#include <cmath>
#include <iostream>
using json = nlohmann::json;

static const std::string CALIBRATIONS_FILE = "calibrations.json";
static const double ATTACKING_BALL_CLOSE_DISTANCE = 35.0;
static const double ATTACKING_BALL_FAR_DISTANCE = 95.0;

AttackerStateAttacking::AttackerStateAttacking(Robot* robot) : State("attacking"), _robot(robot)
{}

AttackerStateAttacking::~AttackerStateAttacking()
{}

void AttackerStateAttacking::doActions()
{
    Vector2D destination;
    Vector2D robotToBall = Global::ball - _robot->getPosition();
    const double distanceToBall = std::hypot(robotToBall.x, robotToBall.y);

    if(distanceToBall > ATTACKING_BALL_FAR_DISTANCE)
        destination = Global::ball;
    else if(distanceToBall > ATTACKING_BALL_CLOSE_DISTANCE)
        destination = AttackerUnivector::ballApproachTarget(_robot->getPosition());
    else
        destination = Global::areaGoalAttack.getCenter();

    if(AttackerUnivector::applyNavigation(_robot, destination, false, false))
        return;

    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft(), _robot->getPwmRight());
}

std::string AttackerStateAttacking::checkConditions()
{
    if(Global::bufferKeyboard == (int)'p')
        return "idle";
    
    if(!WorldModel::isAlignedWith(_robot->getOrientation(), Global::ball - _robot->getPosition()))
        return "seeking";

    return "";
}

void AttackerStateAttacking::entryActions()
{
    std::ifstream arquivo(CALIBRATIONS_FILE);
    if(!arquivo.is_open()){
        std::cerr << "Error opening calibration file '" << CALIBRATIONS_FILE << "' for attacker attacking state." << std::endl;
    }else{
        json j;

        try{
            arquivo>>j;

            double kd = j["Attacker"]["Attacking"]["kd"];
                         

            double kp = j["Attacker"]["Attacking"]["kp"];

            double pwm = j["Attacker"]["Attacking"]["pwm"];

            _robot->setPD(kp, kd); //70,140
            _robot->setBasePwmValue(pwm); //100
        }catch(const std::exception& e){
            std::cerr << "Error reading calibration file '" << CALIBRATIONS_FILE << "': " << e.what() << std::endl;
        }
    }
    Vector2D robotToDestiny = Global::ball - _robot->getPosition();
    _robot->setLastError((_robot->getOrientation()||robotToDestiny));
}

void AttackerStateAttacking::exitActions()
{}
