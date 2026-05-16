#include "CommunicationWIFICalibration.hpp"
#include <cerrno>
#include <cstring>

CommunicationWIFICalibration::CommunicationWIFICalibration(std::string ip,int port){
    
    std::cout << "Tentando conectar via WIFI ... " << std::endl;
    std::memset(_writeBuffer, 0, sizeof(_writeBuffer));
    this->_sockFd = socket(AF_INET, SOCK_DGRAM, 0);	
    if(this->_sockFd == -1){
        std::cerr << "Error: could not create UDP calibration socket: " << std::strerror(errno) << std::endl;
        return;
    }
    int enable = 1;
    if(setsockopt(this->_sockFd, SOL_SOCKET,SO_REUSEADDR,&enable, sizeof(enable)) == -1)
        std::cerr << "Warning: could not set SO_REUSEADDR on UDP calibration socket: " << std::strerror(errno) << std::endl;
	this->_server.sin_family = AF_INET;
    this->_server.sin_addr.s_addr = htons(INADDR_ANY);
	this->_server.sin_port = htons(port);
	if(inet_aton(ip.c_str(), &_server.sin_addr) == 0)
        std::cerr << "Error: invalid UDP calibration IP address '" << ip << "'." << std::endl;
    
    std::cout << "Conectado via WIFI ... " << std::endl;

    if (bind(this->_sockFd, (const struct sockaddr *)&this->_server, sizeof(this->_server)) == -1) {
        std::cerr << "Falha ao vincular (bind). A porta " << port << " pode estar em uso." << std::endl;
        // No Linux, o closesocket é a função close()
        close(this->_sockFd); 
        this->_sockFd = -1;
        return ;
    }
    
    
} 

void CommunicationWIFICalibration::receiveMessage(){
    const int bufferSize = 1024;
    char buffer[bufferSize];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    if(this->_sockFd == -1){
        std::cerr << "Error: UDP calibration listener was not started because socket initialization failed." << std::endl;
        return;
    }
    std::cout<<"UDP Listen - "<<ntohs(_server.sin_port)<< std::endl;
    
    while(true){
        std::memset(buffer,0,bufferSize);
        ssize_t bytes_received = recvfrom(
            this -> _sockFd,
            buffer,
            bufferSize - 1,
            0,
            (struct sockaddr *)&client_addr,
            &client_len
        );

        if(bytes_received < 0){
            std::cerr << "Error: failed to receive UDP calibration packet: " << std::strerror(errno) << std::endl;
            continue;
        }

        buffer[bytes_received] = '\0';
        std::string json_string(buffer);

        jsonHandler.handlePacket(json_string);
        
    }


}
