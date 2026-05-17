#include "Communication.hpp"


Communication::Communication(const std::string port)
{
    _serial.Open(port);
    _serial.SetBaudRate(LibSerial::BaudRate::BAUD_9600);
    _serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    _serial.SetParity( LibSerial::Parity::PARITY_NONE);
    _serial.SetFlowControl( LibSerial::FlowControl::FLOW_CONTROL_NONE );
    _serial.SetStopBits( LibSerial::StopBits::STOP_BITS_1);
    this->_robotsConfigured = false;
    for(int i = 0; i <16; i++)
        _writeBuffer[i] = 0;
}

Communication::~Communication()
{}


void Communication::writeMessage(const int index, const unsigned char pwmLeft, const unsigned char pwmRight, const bool reverseLeft, const bool reverseRight)
{
    std::lock_guard<std::mutex> lock(_writeMutex);
    _writeBuffer[0] = 0x5B;
    int id_mask = index + 1;
    unsigned char maskLeft  = reverseLeft  ? (1 << (2*id_mask + 1)) : ~(1 << (id_mask*2 + 1)); 
    unsigned char maskRight = reverseRight ? (1 << 2*id_mask)     : ~(1 << id_mask*2);
    _writeBuffer[1] = reverseLeft  ? _writeBuffer[1] | maskLeft  : _writeBuffer[1] & maskLeft;
    _writeBuffer[1] = reverseRight ? _writeBuffer[1] | maskRight : _writeBuffer[1] & maskRight;
    _writeBuffer[2 * index + 2] = pwmLeft;
    _writeBuffer[2 * index + 3] = pwmRight;
}

void Communication::sendMessage()
{
    std::lock_guard<std::mutex> lock(_writeMutex);
    _writeBuffer[8] = 1;
    _serial.write(_writeBuffer, 9);
}

void Communication::getMessage()
{
    std::lock_guard<std::mutex> lock(_writeMutex);
    for(int i = 0; i < 16; i++)
        std::cout << std::hex << (unsigned short) _writeBuffer[i] << std::endl;
    std::cout << std::dec << std::endl << std::endl;
}

void Communication::stopAll(){
    for(int i = 0; i <= 2; i++)
        this->writeMessage(i, 0, 0);
}

void Communication::configureRobots(Config config){

    std::thread tread(&Communication::readMessage, this);
    std::unordered_map<std::string, std::pair<HardwareConfig *, bool>> map;    
    if(config.r0.active)
        map[config.r0.hardware.xbee] = std::make_pair<HardwareConfig * , bool>(&config.r0.hardware, false);

    if(config.r1.active)
        map[config.r1.hardware.xbee] = std::make_pair<HardwareConfig *, bool>(&config.r1.hardware, false);
    
    if(config.r2.active)
        map[config.r2.hardware.xbee] = std::make_pair<HardwareConfig *, bool>(&config.r2.hardware, false);
    
    std::cout << "Sending hello to all ... " << std::endl << std::flush;
    _writeBuffer[0] = 0x5C;
    _writeBuffer[1] = 0x01;
    
    _serial.write(_writeBuffer, 2);
    _serial.DrainWriteBuffer();

    this->gotInput = false;
    long long i = 0;
    while(!allRobotsConfigured(map) && i < 1E5){
        memset(_writeBuffer, 0, 16);
        if(_readBuffer[0] == 0x5C && this->gotInput){
            std::cout << "Got a xbee serial ... " << std::endl;
            std::string xbee(_readBuffer, 9);
            xbee = xbee.substr(1, 8);
            for(int i = 1; i < 9; i++)
                std::cout << std::hex << (unsigned char) _readBuffer[i];
            std::cout << std::endl << std::flush;

            if(map.find(xbee) != map.end() && !map[xbee].second){
                std::cout << "Found robot " << map[xbee].first->chassis << ", sending hardware configuration ... " << std::endl << std::flush;
                sendConfigurationToRobot(xbee, map[xbee].first);
            }else
                _serial.FlushInputBuffer();
            memset(_readBuffer, 0, 16);
            this->gotInput = false;
        }else if(_readBuffer[0] == 0x5D && this->gotInput){
            std::cout << "Configuration received: ";
            std::string xbee(_readBuffer, 15);
            xbee = xbee.substr(1, 8);
            std::cout << xbee << std::endl << std::flush;
            map[xbee].second = true;
            memset(_readBuffer, 0, 16);
            this->gotInput = false;

        }else if(_readBuffer[0] != 0 && this->gotInput){
            memset(_readBuffer, 0, 16);
            _serial.FlushInputBuffer();
            this->gotInput = false;
        }
        usleep(1000);
        i++;
        if(i % (long long) 1E4 == 0)
        {
            memset(_writeBuffer, 0, 16);
            std::cout << std::dec << i << " Sending hello to all again ... " << std::endl << std::flush;
            _writeBuffer[0] = 0x5C;
            _writeBuffer[1] = 0x01;
            _serial.write(_writeBuffer, 2);
            _serial.DrainWriteBuffer();
        }
    }
    this->_robotsConfigured = true;
    printRobotsConfigured(map);
    tread.join();
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
            std::cout << i->second.first->chassis << " - OK" << std::endl << std::flush;
        else
            std::cerr << "WARNING: " << i->second.first->chassis << " NOT FOUND!" << std::endl;
}

void Communication::sendConfigurationToRobot(std::string xbee, HardwareConfig * configuration){
    memset(_writeBuffer, 0, 16);
    _writeBuffer[0] = 0x5D;
    for(int i = 0; i < 8; i++)
        _writeBuffer[i+1] = xbee[i];
    _writeBuffer[9]  = (unsigned char) configuration->id + 10;
    _writeBuffer[10] = (unsigned char) configuration->pinMotorEsqA;
    _writeBuffer[11] = (unsigned char) configuration->pinMotorEsqB;
    _writeBuffer[12] = (unsigned char) configuration->pinMotorDirA;
    _writeBuffer[13] = (unsigned char) configuration->pinMotorDirB;
    _writeBuffer[14] = 0x01;
    _serial.write(_writeBuffer, 15);
    _serial.DrainWriteBuffer();
}

void Communication::readMessage(){
    LibSerial::SerialStream * stream = getSerial();
    char * buffer = getBuffer();
    memset(buffer, 0, 16);
    char nextChar;
    std::cout << gotInput << std::endl << std::flush;
    int s = 0;
    while(!_robotsConfigured){
        *stream >> std::noskipws >> nextChar;
        if(nextChar != 0x01 && !gotInput)
            buffer[s++] = (unsigned char) nextChar;
        else if(!gotInput){
            buffer[s] = 0x01;
            s = 0;
            gotInput = true;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(5000));
    }
}

LibSerial::SerialStream * Communication::getSerial(){
    return &this->_serial;
}

char * Communication::getBuffer(){
    return (char *) this->_readBuffer;
}

int Communication::getLeftPwm(int id){
    std::lock_guard<std::mutex> lock(_writeMutex);
    return _writeBuffer[2*id + 2];
}

int Communication::getRightPwm(int id){
    std::lock_guard<std::mutex> lock(_writeMutex);
    return _writeBuffer[2*id + 3];
}
