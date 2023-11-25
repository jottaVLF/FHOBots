//
// Created by fhobots on 9/26/19.
//

#ifndef FHOBOTS_GOALKEEPERSTATEEXITGOAL_HPP
#define FHOBOTS_GOALKEEPERSTATEEXITGOAL_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class GoalkeeperStateExitGoal : public State {
    public:
        GoalkeeperStateExitGoal(Robot* robot);
        ~GoalkeeperStateExitGoal();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:

        double valueX, valueY;
        bool alinhado;
        Robot* _robot;
        bool exitgoalToWaiting();
};


#endif //FHOBOTS_GOALKEEPERSTATEEXITGOAL_HPP
