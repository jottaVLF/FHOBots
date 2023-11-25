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

void MachineState::setState(const std::string nameState)
{
    if(_activeState != nullptr)
        _activeState->exitActions();

    _activeState = _states[nameState];
    _activeState->entryActions();
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
    return _activeState->getName();
}