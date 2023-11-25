#ifndef FHOBOTS_DEFENDERSTATESEEKING_HPP
#define FHOBOTS_DEFENDERSTATESEEKING_HPP

#include "../../model/Robot.hpp"
#include "../State.hpp"

class DefenderStateSeeking : public State
{
    public:
        DefenderStateSeeking(Robot* robot);
        ~DefenderStateSeeking();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        double atkLastX;
        Robot* _robot;
};

#endif //FHOBOTS_DEFENDERSTATESEEKING_HPP
