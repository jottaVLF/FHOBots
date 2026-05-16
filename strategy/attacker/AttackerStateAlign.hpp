#ifndef FHOBOTS_ATTACKERSTATEALIGN_HPP
#define FHOBOTS_ATTACKERSTATEALIGN_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"
#include "../../model/WorldModel.hpp"

class AttackerStateAlign : public State {

public:
    AttackerStateAlign(Robot* robot);
    ~AttackerStateAlign();

    void doActions();
    std::string checkConditions();
    void entryActions();
    void exitActions();

private:

    Robot* _robot;
};


#endif //FHOBOTS_ATTACKERSTATEALIGN_HPP
