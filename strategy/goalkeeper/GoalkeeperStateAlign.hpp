#ifndef FHOBOTS_GOALKEEPERSTATEALIGN_HPP
#define FHOBOTS_GOALKEEPERSTATEALIGN_HPP


#include "../State.hpp"
#include "../../model/Robot.hpp"
#include "../../Global.hpp"
#include "../../model/WorldModel.hpp"
class GoalkeeperStateAlign : public State
{
    public:
        GoalkeeperStateAlign(Robot* robot);
        ~GoalkeeperStateAlign();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        void alignWith(Vector2D objective);
        Robot* _robot;
};


#endif //FHOBOTS_GOALKEEPERSTATEALIGN_HPP
