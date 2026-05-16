#ifndef FHOBOTS_ATTACKERSTATEWAITING_HPP
#define FHOBOTS_ATTACKERSTATEWAITING_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class AttackerStateWaiting : public State
{
    public:
        AttackerStateWaiting(Robot* robot);
        ~AttackerStateWaiting();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        Robot* _robot;
};


#endif //FHOBOTS_ATTACKERSTATEWAITING_HPP
