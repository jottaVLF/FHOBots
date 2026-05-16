#include "CommunicationWIFI.hpp"
#include <cerrno>
#include <cstring>

CommunicationWIFI::CommunicationWIFI(std::string ip, int port){
    std::cout << "Tentando conectar via WIFI ... " << std::endl;
    std::memset(_writeBuffer, 0, sizeof(_writeBuffer));
    this->_sockFd = socket(AF_INET, SOCK_DGRAM, 0);	
    if(this->_sockFd == -1){
        std::cerr << "Error: could not create UDP command socket: " << std::strerror(errno) << std::endl;
        return;
    }
    int enable = 1;
    if(setsockopt(this->_sockFd, SOL_SOCKET,SO_BROADCAST,&enable, sizeof(int)) == -1)
        std::cerr << "Warning: could not enable UDP broadcast: " << std::strerror(errno) << std::endl;
	this->_server.sin_family = AF_INET;
	this->_server.sin_port = htons(port);
	if(inet_aton(ip.c_str(), &_server.sin_addr) == 0)
        std::cerr << "Error: invalid UDP command IP address '" << ip << "'." << std::endl;
    std::cout << "Conectado via WIFI ... " << std::endl;
    
}

static unsigned char clampPwm(int pwm)
{
    if(pwm < 0)
        return 0;
    if(pwm > 255)
        return 255;
    return static_cast<unsigned char>(pwm);
}

void CommunicationWIFI::writeMessage(const int index, const int pwmLeft, const int pwmRight, const bool reverseLeft, const bool reverseRight){
    if(index < 0 || index > 2){
        std::cerr << "Warning: ignoring command for invalid robot index " << index << "." << std::endl;
        return;
    }

    //std::cout << "MSG " << (int) pwmLeft << "\t" << (int) pwmRight << std::endl;
    _writeBuffer[0] = 0x5B;
    int id_mask = index + 1;
    unsigned char maskLeft  = reverseLeft  ? (1 << (2*id_mask + 1)) : ~(1 << (id_mask*2 + 1)); 
    unsigned char maskRight = reverseRight ? (1 << 2*id_mask)       : ~(1 << id_mask*2);
    _writeBuffer[1] = reverseLeft  ? _writeBuffer[1] | maskLeft  : _writeBuffer[1] & maskLeft;
    _writeBuffer[1] = reverseRight ? _writeBuffer[1] | maskRight : _writeBuffer[1] & maskRight;
    _writeBuffer[2 * index + 2] = clampPwm(pwmLeft);
    _writeBuffer[2 * index + 3] = clampPwm(pwmRight);
}

void CommunicationWIFI::sendMessage(){
    if(_sockFd == -1)
        return;

    _writeBuffer[8] = 1;
    socklen_t sizeSock=sizeof(_server);
    if(sendto(_sockFd, _writeBuffer, 255, 0, (sockaddr *) &_server, sizeSock) == -1)
        std::cerr << "Error: could not send UDP command packet: " << std::strerror(errno) << std::endl;
}

void CommunicationWIFI::stopAll(){
    for(int i = 0; i <= 2; i++)
        this->writeMessage(i, 0, 0);
}

void CommunicationWIFI::getMessage()
{
    for(int i = 0; i <= 8; i++)
        std::cout << std::hex << (unsigned short) _writeBuffer[i] << std::endl;
    std::cout << std::dec << std::endl << std::endl;
}

int CommunicationWIFI::getLeftPwm(int id){
    return _writeBuffer[2*id + 2];
}

int CommunicationWIFI::getRightPwm(int id){
    return _writeBuffer[2*id + 3];
}
