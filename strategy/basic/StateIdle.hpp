#ifndef FHOBOTS_STATEIDLE_HPP
#define FHOBOTS_STATEIDLE_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class StateIdle : public State
{
    public:
        StateIdle(Robot* robot);
        ~StateIdle();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        Robot* _robot;
};


#endif //FHOBOTS_STATEIDLE_HPP
