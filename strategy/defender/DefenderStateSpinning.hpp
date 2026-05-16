
#ifndef FHOBOTS_DEFENDERSTATESPINNING_HPP
#define FHOBOTS_DEFENDERSTATESPINNING_HPP
#include "../../model/WorldModel.hpp"
#include "../../model/Robot.hpp"
#include "../State.hpp"

class DefenderStateSpinning : public State{

public:
    DefenderStateSpinning(Robot* robot);
    ~DefenderStateSpinning();

    void doActions();
    std::string checkConditions();
    void entryActions();
    void exitActions();

private:

    Robot* _robot;
    bool spinningToSeeking();
    int timer;

};


#endif //FHOBOTS_DEFENDERSTATESPINNING_HPP
