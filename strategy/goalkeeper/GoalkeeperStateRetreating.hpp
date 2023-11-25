#ifndef FHOBOTS_GOALKEEPERSTATERETREATING_HPP
#define FHOBOTS_GOALKEEPERSTATERETREATING_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class GoalkeeperStateRetreating : public State
{
    public:
        GoalkeeperStateRetreating(Robot* robot);
        ~GoalkeeperStateRetreating();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        double valueX;
        bool retreatingToWaiting();
        Robot* _robot;
};


#endif //FHOBOTS_GOALKEEPERSTATERETREATING_HPP
