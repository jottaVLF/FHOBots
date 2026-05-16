#ifndef FHOBOTS_DEFENDERSTATEALIGN_HPP
#define FHOBOTS_DEFENDERSTATEALIGN_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class DefenderStateAlign : public State
{
    public:
        DefenderStateAlign(Robot* robot);
        ~DefenderStateAlign();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        bool align;
        Robot* _robot;
};


#endif //FHOBOTS_DEFENDERSTATEALIGN_HPP
