#ifndef FHOBOTS_ATTACKERSTATESEEKING_HPP
#define FHOBOTS_ATTACKERSTATESEEKING_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class AttackerStateSeeking : public State
{
    public:
        AttackerStateSeeking(Robot* robot);
        ~AttackerStateSeeking();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        Robot* _robot;
};


#endif //FHOBOTS_ATTACKERSTATESEEKING_HPP
