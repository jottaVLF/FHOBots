#ifndef FHOBOTS_GOALKEEPERSTATETURNAROUND_HPP
#define FHOBOTS_GOALKEEPERSTATETURNAROUND_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class GoalkeeperStateTurnaround : public State
{
    public:
        GoalkeeperStateTurnaround(Robot* robot);
        ~GoalkeeperStateTurnaround();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        Robot* _robot;
};


#endif //FHOBOTS_GOALKEEPERSTATETURNAROUND_HPP
