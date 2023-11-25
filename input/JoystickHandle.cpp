#include "JoystickHandle.hpp"

JoystickHandle::JoystickHandle(JoystickSimulationInterface * simulator){
    this->simulator = simulator;
    this->initializeAllegro();
}

void JoystickHandle::initializeAllegro(){
    if(!al_init()){
        std::cout << "Erro ao inicializar o allegro" << std::endl;
        exit(0);
    }

    if(!al_install_joystick()){
        std::cout << "Erro ao inicializar o joystick" << std::endl;
        exit(0);
    }

    if(al_get_num_joysticks() == 0){
        std::cout << "Nenhum joystick encontrado" << std::endl;
        exit(0);
    }
    this->events = al_create_event_queue();

    if(events == NULL){
        std::cout << "Não foi possível criar a fila de eventos" << std::endl;
        exit(0);
    }

    al_register_event_source(this->events, al_get_joystick_event_source());
}

void JoystickHandle::handleJoystickInputs(){
    if(!al_is_event_queue_empty(this->events)){
      ALLEGRO_EVENT event;
      al_wait_for_event(events, &event);
      int inc_x = 0, inc_y = 0;
      if (event.type == ALLEGRO_EVENT_JOYSTICK_AXIS)
      {
        if (event.joystick.axis == 0)
          inc_x = event.joystick.pos;
        if (event.joystick.axis == 1)
          inc_y = event.joystick.pos;
        std::cout << inc_x << " " << inc_y << std::endl;
      }
      this->simulator->updateJoystickDestination(inc_x, inc_y);
    }
}