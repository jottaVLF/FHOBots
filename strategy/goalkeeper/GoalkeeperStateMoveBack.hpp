//
// Created by fhobots on 8/22/19.
//

#ifndef FHOBOTS_GOALKEEPERSTATEMOVEBACK_HPP
#define FHOBOTS_GOALKEEPERSTATEMOVEBACK_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"
#include "../../model/WorldModel.hpp"
#include "../../Global.hpp"
#include <cmath>
#include <iostream>


class GoalkeeperStateMoveBack : public State{
public:
    GoalkeeperStateMoveBack(Robot* robot);
    ~GoalkeeperStateMoveBack();

    void doActions();
    std::string checkConditions();
    void entryActions();
    void exitActions();

private:
    Robot* _robot;

};


#endif //FHOBOTS_GOALKEEPERSTATEMOVEBACK_HPP
