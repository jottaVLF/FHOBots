//
// Created by wolves on 19/09/19.
//

#ifndef FHOBOTS_ATTACKERSTATESPINNING_HPP
#define FHOBOTS_ATTACKERSTATESPINNING_HPP

#include "../../model/Robot.hpp"
#include "../State.hpp"

class AttackerStateSpinning : public State{

public:
    AttackerStateSpinning(Robot* robot);
    ~AttackerStateSpinning();

    void doActions();
    std::string checkConditions();
    void entryActions();
    void exitActions();

private:

    Robot* _robot;
    bool spinningToSeeking();


};


#endif //FHOBOTS_ATTACKERSTATESPINNING_HPP
