//
// Created by fhobots on 8/22/19.
//

#ifndef FHOBOTS_GOALKEEPERMOVEFORWARD_HPP
#define FHOBOTS_GOALKEEPERMOVEFORWARD_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class GoalkeeperMoveForward : public State{
public:
    GoalkeeperMoveForward(Robot* robot);
    ~GoalkeeperMoveForward();

    void doActions();
    std::string checkConditions();
    void entryActions();
    void exitActions();

private:
    double valueX;

    Robot* _robot;
    bool forwardToWaiting();
    bool forwardToWaitingLimit();
    bool forwardToBack();


};


#endif //FHOBOTS_GOALKEEPERMOVEFORWARD_HPP
