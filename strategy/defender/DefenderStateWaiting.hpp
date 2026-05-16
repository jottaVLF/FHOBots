#ifndef FHOBOTS_DEFENDERSTATEWAITING_HPP
#define FHOBOTS_DEFENDERSTATEWAITING_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"
#include "../../model/WorldModel.hpp"
#include "../../Global.hpp"

class DefenderStateWaiting : public State
{
    public:
        DefenderStateWaiting(Robot* robot);
        ~DefenderStateWaiting();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();
        int contador;
    private:
        int _posFixedX;
        Robot* _robot;
};


#endif //FHOBOTS_DEFENDERSTATEWAITING_HPP
