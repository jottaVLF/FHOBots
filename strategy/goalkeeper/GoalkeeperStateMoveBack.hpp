//
// Created by fhobots on 8/22/19.
//

#ifndef FHOBOTS_GOALKEEPERSTATEMOVEBACK_HPP
#define FHOBOTS_GOALKEEPERSTATEMOVEBACK_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class GoalkeeperStateMoveBack : public State{
public:
    GoalkeeperStateMoveBack(Robot* robot);
    ~GoalkeeperStateMoveBack();

    void doActions();
    std::string checkConditions();
    void entryActions();
    void exitActions();

private:
    double valueX;

    Robot* _robot;
    bool backToWaiting();
    bool backToWaitingLimit();
    bool backToForward();
    bool backToBackOff();

};


#endif //FHOBOTS_GOALKEEPERSTATEMOVEBACK_HPP
