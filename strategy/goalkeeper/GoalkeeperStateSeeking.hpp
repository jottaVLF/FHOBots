#ifndef FHOBOTS_GOALKEEPERSTATESEEKING_HPP
#define FHOBOTS_GOALKEEPERSTATESEEKING_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"
#include "../../model/WorldModel.hpp"

class GoalkeeperStateSeeking : public State
{
    public:
        GoalkeeperStateSeeking(Robot* robot);
        ~GoalkeeperStateSeeking();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        Robot* _robot;
};


#endif //FHOBOTS_GOALKEEPERSTATESEEKING_HPP
