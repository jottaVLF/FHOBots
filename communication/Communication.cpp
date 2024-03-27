#include "Communication.hpp"


Communication::Communication(const std::string port)
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


void Communication::writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight, const bool reverseLeft, const bool reverseRight)
{
    _message[0] = 0x5B;
    
    unsigned char maskLeft  = reverseLeft  ? (1 << 2*index + 1) : ~(1 << index*2 + 1); 
    unsigned char maskRight = reverseRight ? (1 << 2*index)     : ~(1 << index*2);
    _message[1] = reverseLeft  ? _message[1] | maskLeft  : _message[1] & maskLeft;
    _message[1] = reverseRight ? _message[1] | maskRight : _message[1] & maskRight;
    
    _message[2 * index + 1] = pwmLeft;
    _message[2 * index + 2] = pwmRight;
}

void Communication::sendMessage()
{
    _serial.write(_message, 9);
}

std::string Communication::getMessage()
{
    std::string buffer(_message);
    return buffer;
}

void Communication::stopAll(){
    for(int i = 0; i <= 2; i++)
        this->writeMessage(i, 0, 0);
}

void Communication::configureRobots(Config config){
    
    std::unordered_map<std::string, std::pair<HardwareConfig *, bool>> map;    
    if(config.r0.active)
        map[config.r0.hardware.xbee] = std::make_pair<HardwareConfig * , bool>(&config.r0.hardware, false);

    if(config.r1.active)
        map[config.r1.hardware.xbee] = std::make_pair<HardwareConfig *, bool>(&config.r1.hardware, false);
    
    if(config.r2.active)
        map[config.r2.hardware.xbee] = std::make_pair<HardwareConfig *, bool>(&config.r2.hardware, false);
    
    std::cout << "Sending hello to all ... " << std::endl;
    _message[0] = 0x5C;
    _message[1] = 0x01;
    
    _serial.write(_message, 2);
    _serial.DrainWriteBuffer();

    long long i = 0;
    while(!allRobotsConfigured(map) && i < 1E5){
        memset(_message, 0, 16);
        _serial.read(_message, 16);
        if(_message[0] == 0x5C){
            std::cout << "Got a xbee serial ... " << std::endl;
            std::string xbee(_message, 9);
            xbee = xbee.substr(1, 8);
            
            for(int i = 1; i < 9; i++)
                std::cout << std::hex << (unsigned char) _message[i];
            std::cout << std::endl;

            if(map.find(xbee) != map.end() && !map[xbee].second){
                map[xbee].second = true;
                std::cout << "Found robot " << map[xbee].first->chassis << ", sending hardware configuration ... " << std::endl;
                sendConfigurationToRobot(xbee, map[xbee].first);
            }else
                _serial.FlushInputBuffer();
        }else if(_message[0] == 0x5D){
            std::cout << "Configuration received: ";
            for(int i = 0; i < 15; i++)
                std::cout << std::hex << (unsigned char) _message[i] << " ";
            std::cout << std::endl;
        }
        usleep(1000);
        i++;
        if(i % (long long) 1E3 == 0)
        {
            memset(_message, 0, 16);
            std::cout << "Sending hello to all again ... " << std::endl;
            _message[0] = 0x5C;
            _message[1] = 0x01;
            _serial.write(_message, 2);
        }
    }
    printRobotsConfigured(map);
}

bool Communication::allRobotsConfigured(std::unordered_map<std::string, std::pair<HardwareConfig *, bool>> map){

    bool isAllConfigured = true;
    for(auto i = map.begin(); i != map.end(); i++)
        isAllConfigured = isAllConfigured && i->second.second;
    
    return isAllConfigured;
}

void Communication::printRobotsConfigured(std::unordered_map<std::string, std::pair<HardwareConfig *, bool>> map){

    for(auto i = map.begin(); i != map.end(); i++)
        if(i->second.second)
            std::cout << i->second.first->chassis << " - OK" << std::endl;
        else
            std::cerr << "WARNING: " << i->second.first->chassis << " NOT FOUND!" << std::endl;
}

void Communication::sendConfigurationToRobot(std::string xbee, HardwareConfig * configuration){
    memset(_message, 0, 16);
    _message[0] = 0x5D;
    for(int i = 0; i < 8; i++)
        _message[i+1] = xbee[i];
    _message[9]  = (unsigned char) configuration->id + 10;
    _message[10] = (unsigned char) configuration->pinMotorEsqA;
    _message[11] = (unsigned char) configuration->pinMotorEsqB;
    _message[12] = (unsigned char) configuration->pinMotorDirA;
    _message[13] = (unsigned char) configuration->pinMotorDirB;
    _message[14] = 0x01;
    _serial.write(_message, 15);
    _serial.DrainWriteBuffer();
}