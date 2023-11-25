#ifndef FHOBOTS_MACHINESTATE_HPP
#define FHOBOTS_MACHINESTATE_HPP

#include <map>
#include "State.hpp"

class MachineState {

    public:

        /// Construtores
        MachineState();
        ~MachineState();

        /**
         * Adiciona um novo estado na maquina de estado
         * */
        void addState(State *state);

        /**
         * Define um novo estado atual
         * */
        void setState(const std::string nameState);

        /**
         * Executa as ações do estado e verifica se deve haver mudança de estado
         * */
        void think();

        /**
         * Retorna o estado atual
         * */
        const std::string currentState();

    private:

        std::map<std::string, State*> _states;
        State* _activeState;
};


#endif //FHOBOTS_MACHINESTATE_HPP
