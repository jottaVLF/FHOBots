#ifndef FHOBOTS_JOYSTICK_HPP
#define FHOBOTS_JOYSTICK_HPP

#include <allegro5/allegro5.h>
#include <iostream>
#include "JoystickSimulationInterface.hpp"

class JoystickHandle{
private:
    ALLEGRO_EVENT_QUEUE * events = NULL;
    JoystickSimulationInterface * simulator = NULL;
public:
    JoystickHandle(JoystickSimulationInterface * simulator);
    void initializeAllegro();
    void handleJoystickInputs();
};

#endif