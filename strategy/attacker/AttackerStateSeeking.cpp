#include "AttackerStateSeeking.hpp"
#include "AttackerUnivector.hpp"
#include "../../communication/json.hpp"
using json = nlohmann::json;

static const std::string CALIBRATIONS_FILE = "calibrations.json";

AttackerStateSeeking::AttackerStateSeeking(Robot* robot) : State("seeking"), _robot(robot)
{}

AttackerStateSeeking::~AttackerStateSeeking()
{}

void AttackerStateSeeking::doActions()
{   
    Vector2D destination = Global::ball;
    if(AttackerUnivector::applyNavigation(_robot, destination, true, true))
        return;

    _robot->calculatePwm(destination);
    Global::communication->writeMessage(_robot->getPosMessage(), _robot->getPwmLeft()-10, _robot->getPwmRight());
    
    
}

std::string AttackerStateSeeking::checkConditions()
{
    Vector2D ballToRobot = Global::ball - _robot->getPosition();   
    bool isAlignedWithBall =        WorldModel::isAlignedWith(_robot->getOrientation(), Global::ball - _robot->getPosition());
    bool isAlignedWithAttackGoal =  WorldModel::isAlignedWith(_robot->getOrientation(), Global::areaGoalAttack - _robot->getPosition());

    

    if(Global::bufferKeyboard == (int)'p')
        return "idle";

    

   /*if(Global::bufferKeyboard == 32)
        return "attacking";

        
    /*    
        
    if(fabs(_robot->getErrorAngleTo(ballToRobot)) > M_PI/2)
        //return "align";

    if(WorldModel::isDeffenseFieldOnLeft() && 
    WorldModel::isFacing(_robot->getOrientation(),LEFT) &&
     _robot->getPosition().x < 5*Global::fieldRect.width/8  
    )
        //return "spinning";

        */

    if(isAlignedWithBall && isAlignedWithAttackGoal)
        return "attacking";
    
    /*

    if(WorldModel::isAlignedWithWallAndBall(_robot->getPosition(), _robot->getOrientation()))
        return "spinning";   

    if(WorldModel::isOnDeffenseField(_robot->getPosition()) &&
       WorldModel::isOnDeffenseField(Global::ball) && 
       WorldModel::nearstRobotTo(Global::ball) != _robot)
        return "waiting";
    
    */


    if(WorldModel::isAlignedWithWall(_robot->getPosition(), _robot->getOrientation())&&
        !WorldModel::isInAttackArea(_robot->getPosition()))
        return "backoff";

    if(WorldModel::isInsideDeffenseArea(_robot->getPosition()) && 
    WorldModel::isFacingArea(_robot->getOrientation(), Global::areaToDeffend))
        return "backoff";

    /*

    if(WorldModel::isInsideDeffenseArea(Global::ball) && !WorldModel::isInsideDeffenseArea(_robot->getPosition()))
        return "waiting";

    */

    return ""; 
}

void AttackerStateSeeking::entryActions()
{
    std::ifstream arquivo(CALIBRATIONS_FILE);
    if(!arquivo.is_open()){
        std::cerr << "Error opening calibration file '" << CALIBRATIONS_FILE << "' for attacker seeking state." << std::endl;
    }else{
        json j;

        try{
            arquivo>>j;

            double kd = j["Attacker"]["Seeking"]["kd"];
                         
            double kp = j["Attacker"]["Seeking"]["kp"];

            double pwm = j["Attacker"]["Seeking"]["pwm"];

            //sstd::cout<<kd<<" "<<kp<<" "<<" "<<pwm<<std::endl;

            Vector2D dist = Global::ball - _robot->getPosition();
            _robot->setPD(kp,kd); //12,30
            _robot->setBasePwmValue(pwm);//80
        }catch(const std::exception& e){
            std::cerr << "Error reading calibration file '" << CALIBRATIONS_FILE << "': " << e.what() << std::endl;
        }
    }
    _robot->setMaxPwm(255);
    _robot->lastPos = _robot->getPosition();
    _robot->lastOri = _robot->getOrientation();
}

void AttackerStateSeeking::exitActions()
{}
