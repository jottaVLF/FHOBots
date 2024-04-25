//
// Created by fhobots on 8/22/19.
//

#ifndef FHOBOTS_GOALKEEPERSTATESPINNING_HPP
#define FHOBOTS_GOALKEEPERSTATESPINNING_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"
#include "../../model/WorldModel.hpp"

class GoalkeeperStateSpinning : public State{
public:
    GoalkeeperStateSpinning(Robot* robot);
    ~GoalkeeperStateSpinning();

    void doActions();
    std::string checkConditions();
    void entryActions();
    void exitActions();

private:

    Robot* _robot;
    bool spinningToWaiting();
};


#endif //FHOBOTS_GOALKEEPERSTATESPINNING_HPP
