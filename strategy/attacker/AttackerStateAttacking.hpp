#ifndef FHOBOTS_ATTACKERSTATEATTACKING_HPP
#define FHOBOTS_ATTACKERSTATEATTACKING_HPP
#include <fstream>
#include "../State.hpp"
#include "../../model/Robot.hpp"
#include "../../model/WorldModel.hpp"

class AttackerStateAttacking : public State {

    public:
        AttackerStateAttacking(Robot* robot);
        ~AttackerStateAttacking();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:

        Robot* _robot;
};


#endif //FHOBOTS_ATTACKERSTATEATTACKING_HPP
