//
// Created by fhobots on 8/22/19.
//

#ifndef FHOBOTS_GOALKEEPERSTATEWAITING_HPP
#define FHOBOTS_GOALKEEPERSTATEWAITING_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class GoalkeeperStateWaiting : public State{
public:
    GoalkeeperStateWaiting(Robot* robot);
    ~GoalkeeperStateWaiting();

    void doActions();
    std::string checkConditions();
    void entryActions();
    void exitActions();

private:

    Robot* _robot;
    bool waitingToMoveBack();
    bool waitingToMoveBackLimit(int kind);
    double valueX;
    bool farFromWall;

    bool waitingMoveToForward();
    bool waitingMoveToForwardLimit(int kind);

    bool waitingToSpinning();
    bool waitingToKicking();

    bool waitingToReturn();
};


#endif //FHOBOTS_GOALKEEPERSTATEWAITING_HPP
