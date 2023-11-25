//
// Created by helielton on 16/09/18.
//

#ifndef FHOBOTS_STATE_HPP
#define FHOBOTS_STATE_HPP

#include <string>

class State {

    public:

        State(std::string name) : _name(name)
        {}

        virtual ~State()
        {}

        std::string getName()
        { return _name; }

        virtual void doActions() = 0;
        virtual std::string checkConditions() = 0;
        virtual void entryActions() = 0;
        virtual void exitActions() = 0;

    private:

        std::string _name;
};


#endif //FHOBOTS_STATE_HPP
