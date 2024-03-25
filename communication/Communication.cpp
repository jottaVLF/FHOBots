#include "Communication.hpp"
#include <SerialPortConstants.h>
#include <sstream>
#include <cstring>
#include <iostream>

Communication::Communication(const std::string port) : _message{0x5b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
{
    _serial.Open(port);
    _serial.SetBaudRate(LibSerial::BaudRate::BAUD_9600);
    _serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    _serial.SetParity( LibSerial::Parity::PARITY_NONE);
    _serial.SetFlowControl( LibSerial::FlowControl::FLOW_CONTROL_NONE );
    _serial.SetStopBits( LibSerial::StopBits::STOP_BITS_1);
}

Communication::~Communication()
{}


void Communication::writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight)
{
    _message[2 * index + 1] = pwmLeft;
    _message[2 * index + 2] = pwmRight;
}

void Communication::sendMessage()
{
    if(_message[1] %2 != 0)
        _message[1] ++;
    if(_message[2] %2 != 0)
        _message[2] ++;
    for(int i = 1; i < 7; i++)
        std::cout << (int) _message[i] << " ";
    std::cout << std::endl;
    _serial.write(_message, 8);
}

std::string Communication::getMessage()
{
    char buffer[30];
    snprintf(buffer,30,"[%d %d, %d %d, %d %d]",_message[1],_message[2],_message[3],_message[4],_message[5],_message[6]);
    return buffer;
}

void Communication::stopAll(){
    for(int i = 0; i <= 2; i++)
        this->writeMessage(i, 0, 0);
}

void Communication::configureRobots(HardwareConfig hardwareConfig){

    

}