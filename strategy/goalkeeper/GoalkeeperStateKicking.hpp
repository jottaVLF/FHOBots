#ifndef FHOBOTS_GOALKEEPERSTATEKICKING_HPP
#define FHOBOTS_GOALKEEPERSTATEKICKING_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class GoalkeeperStateKicking : public State
{
    public:
        GoalkeeperStateKicking(Robot* robot);
        ~GoalkeeperStateKicking();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        Robot* _robot;

        bool kickingToWaiting();
};


#endif //FHOBOTS_GOALKEEPERSTATEKICKING_HPP
