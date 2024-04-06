#ifndef FHOBOTS_STATEBACKOFF_HPP
#define FHOBOTS_STATEBACKOFF_HPP

#include "../State.hpp"
#include "../../model/Robot.hpp"
#include "../../model/WorldModel.hpp"

class StateBackOff : public State
{  
    public:
        StateBackOff(Robot* robot);
        ~StateBackOff();

        void doActions();
        std::string checkConditions();
        void entryActions();
        void exitActions();
 
        double newAngle;
        bool alinhado;
        int _side; /// 1 = BATEU NA PAREDE DO EIXO X, 0 = BATEU NA PAREDE DO EIXO Y
        Robot* _robot;
};

#endif //FHOBOTS_STATEBACKOFF_HPP