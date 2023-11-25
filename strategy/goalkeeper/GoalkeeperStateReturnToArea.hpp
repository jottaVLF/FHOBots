//
// Created by fhobots on 9/26/19.
//

#ifndef FHOBOTS_GOALKEEPERSTATERETURNTOAREA_HPP
#define FHOBOTS_GOALKEEPERSTATERETURNTOAREA_HPP

#include "../../model/Robot.hpp"
#include "../State.hpp"

class GoalkeeperStateReturnToArea : public State {
    public:
        GoalkeeperStateReturnToArea(Robot* robot);
        ~GoalkeeperStateReturnToArea();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        double valueY;
        double valueX;
        bool alinhado;
        bool returnToWaiting();
        Robot* _robot;
};


#endif //FHOBOTS_GOALKEEPERSTATERETURNTOAREA_HPP
