//
// Created by fhobots on 9/26/19.
//

#ifndef FHOBOTS_GOALKEEPERSTATEEXITGOAL_HPP
#define FHOBOTS_GOALKEEPERSTATEEXITGOAL_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"
#include "../../model/WorldModel.hpp"
#include "../../Global.hpp"

class GoalkeeperStateExitGoal : public State {
    public:
        GoalkeeperStateExitGoal(Robot* robot);
        ~GoalkeeperStateExitGoal();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        Robot* _robot;
};


#endif //FHOBOTS_GOALKEEPERSTATEEXITGOAL_HPP
