#include "MachineState.hpp"
#include <iostream>

MachineState::MachineState()
{
    _states.clear();
    _activeState = nullptr;
}

MachineState::~MachineState()
{
    for(auto it = _states.begin(); it != _states.end(); it++)
        delete it->second;

    _states.clear();
    _activeState = nullptr;
}

void MachineState::addState(State *state)
{
    _states[state->getName()] = state;
}

bool MachineState::setState(const std::string nameState)
{
    auto state = _states.find(nameState);
    if(state == _states.end()){
        std::cerr << "State not found: " << nameState << std::endl;
        return false;
    }

    if(_activeState != nullptr && _activeState->getName() == nameState)
        return true;

    if(_activeState != nullptr)
        _activeState->exitActions();

    _activeState = state->second;
    _activeState->entryActions();
    return true;
}

void MachineState::think()
{
    if(_activeState == nullptr)
        return;

    _activeState->doActions();
    std::string newNameState = _activeState->checkConditions();
    
    if(newNameState.size() != 0){
        setState(newNameState);}
}

const std::string MachineState::currentState()
{
    if(_activeState == nullptr)
        return "";

    return _activeState->getName();
}
