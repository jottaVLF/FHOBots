//
// Created by fhobots on 10/3/19.
//

#ifndef FHOBOTS_STATEEXITAREA_HPP
#define FHOBOTS_STATEEXITAREA_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"

class StateExitArea : public State {

    public:
        StateExitArea(Robot* robot);
        ~StateExitArea();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();

    private:
        double valueX, valueY;
        Vector2D destination;
        Robot* _robot;

};


#endif //FHOBOTS_STATEEXITAREA_HPP
