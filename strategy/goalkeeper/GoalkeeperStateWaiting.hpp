//
// Created by fhobots on 8/22/19.
//

#ifndef FHOBOTS_GOALKEEPERSTATEWAITING_HPP
#define FHOBOTS_GOALKEEPERSTATEWAITING_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"
#include "../../Global.hpp"
#include <cmath>
#include <iostream>
#include "../../model/WorldModel.hpp"

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
    double valueX;
    bool farFromWall;
};


#endif //FHOBOTS_GOALKEEPERSTATEWAITING_HPP
