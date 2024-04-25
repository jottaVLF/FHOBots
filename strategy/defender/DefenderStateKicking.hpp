#ifndef FHOBOTS_DEFENDERSTATEKICKING_HPP
#define FHOBOTS_DEFENDERSTATEKICKING_HPP

#include "../../model/WorldModel.hpp"
#include "../../model/Robot.hpp"
#include "../State.hpp"

class DefenderStateKicking : public State
{
    public:
        DefenderStateKicking(Robot* robot);
        ~DefenderStateKicking();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        Robot* _robot;
};


#endif //FHOBOTS_DEFENDERSTATEKICKING_HPP
