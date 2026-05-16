#include "CommunicationESPNow.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>

namespace
{
    speed_t baudRateToTermiosSpeed(int baudRate)
    {
        switch(baudRate)
        {
            case 9600:
                return B9600;
            case 19200:
                return B19200;
            case 38400:
                return B38400;
            case 57600:
                return B57600;
            case 115200:
                return B115200;
#ifdef B230400
            case 230400:
                return B230400;
#endif
#ifdef B460800
            case 460800:
                return B460800;
#endif
#ifdef B921600
            case 921600:
                return B921600;
#endif
            default:
                std::cerr << "Warning: unsupported serial baud rate " << baudRate
                          << ". Falling back to 115200." << std::endl;
                return B115200;
        }
    }
}

CommunicationESPNow::CommunicationESPNow(const std::string &serialPort, int baudRate)
    : _serialPort(serialPort), _baudRate(baudRate), _serialFd(-1), _packetCounter(0)
{
    _packet.fill(0);

    for(int i = 0; i < ROBOT_COUNT; i++)
        _commands[i] = RobotCommand{static_cast<uint8_t>(i), 0, 0, 0};

    rebuildPacket();

    std::cout << "Initializing ESP-NOW USB serial transport at "
              << _serialPort << " (" << _baudRate << " baud)..." << std::endl;

    if(openSerialPort())
        std::cout << "ESP-NOW USB serial transport ready at " << _serialPort << "." << std::endl;
}

CommunicationESPNow::~CommunicationESPNow()
{
    if(_serialFd != -1)
        close(_serialFd);
}

bool CommunicationESPNow::openSerialPort()
{
    _serialFd = open(_serialPort.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if(_serialFd == -1)
    {
        std::cerr << "Error: could not open ESP-NOW USB serial port '"
                  << _serialPort << "': " << std::strerror(errno) << std::endl;
        return false;
    }

    if(!configureSerialPort())
    {
        close(_serialFd);
        _serialFd = -1;
        return false;
    }

    tcflush(_serialFd, TCIOFLUSH);
    return true;
}

bool CommunicationESPNow::configureSerialPort()
{
    termios tty;
    if(tcgetattr(_serialFd, &tty) != 0)
    {
        std::cerr << "Error: could not read serial configuration from '"
                  << _serialPort << "': " << std::strerror(errno) << std::endl;
        return false;
    }

    const speed_t speed = baudRateToTermiosSpeed(_baudRate);
    if(cfsetispeed(&tty, speed) != 0 || cfsetospeed(&tty, speed) != 0)
    {
        std::cerr << "Error: could not configure baud rate for '"
                  << _serialPort << "': " << std::strerror(errno) << std::endl;
        return false;
    }

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CLOCAL;
    tty.c_cflag |= CREAD;
#ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS;
#endif

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;

    if(tcsetattr(_serialFd, TCSANOW, &tty) != 0)
    {
        std::cerr << "Error: could not apply serial configuration to '"
                  << _serialPort << "': " << std::strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool CommunicationESPNow::writeAll(const uint8_t *data, std::size_t size)
{
    std::size_t written = 0;
    while(written < size)
    {
        const ssize_t result = write(_serialFd, data + written, size - written);
        if(result < 0)
        {
            if(errno == EINTR)
                continue;

            std::cerr << "Error: could not write ESP-NOW USB serial packet to '"
                      << _serialPort << "': " << std::strerror(errno) << std::endl;
            return false;
        }

        if(result == 0)
        {
            std::cerr << "Error: serial write returned zero bytes for '"
                      << _serialPort << "'." << std::endl;
            return false;
        }

        written += static_cast<std::size_t>(result);
    }

    return true;
}

uint8_t CommunicationESPNow::clampPwm(int pwm)
{
    if(pwm < 0)
        return 0;
    if(pwm > 255)
        return 255;
    return static_cast<uint8_t>(pwm);
}

uint8_t CommunicationESPNow::directionFlags(bool reverseLeft, bool reverseRight)
{
    uint8_t flags = 0;
    if(reverseLeft)
        flags |= 0x01;
    if(reverseRight)
        flags |= 0x02;
    return flags;
}

uint8_t CommunicationESPNow::calculateChecksum()
{
    uint8_t checksum = 0;
    for(int i = 0; i < PACKET_SIZE - 1; i++)
        checksum ^= _packet[i];
    return checksum;
}

void CommunicationESPNow::rebuildPacket()
{
    _packet.fill(0);
    _packet[0] = HEADER_0;
    _packet[1] = HEADER_1;
    _packet[2] = static_cast<uint8_t>(_packetCounter & 0xFF);
    _packet[3] = static_cast<uint8_t>((_packetCounter >> 8) & 0xFF);

    int offset = 4;
    for(int i = 0; i < ROBOT_COUNT; i++)
    {
        _packet[offset++] = _commands[i].id;
        _packet[offset++] = _commands[i].pwmLeft;
        _packet[offset++] = _commands[i].pwmRight;
        _packet[offset++] = _commands[i].flags;
    }

    _packet[16] = 0x00;
    _packet[17] = calculateChecksum();
}

void CommunicationESPNow::setCommandUnlocked(const int index, const int pwmLeft, const int pwmRight, const bool reverseLeft, const bool reverseRight)
{
    _commands[index].id = static_cast<uint8_t>(index);
    _commands[index].pwmLeft = clampPwm(pwmLeft);
    _commands[index].pwmRight = clampPwm(pwmRight);
    _commands[index].flags = directionFlags(reverseLeft, reverseRight);
}

void CommunicationESPNow::writeMessage(const int index, const int pwmLeft, const int pwmRight, const bool reverseLeft, const bool reverseRight)
{
    std::lock_guard<std::mutex> lock(_commMutex);

    if(index < 0 || index >= ROBOT_COUNT)
    {
        std::cerr << "Warning: ignoring command for invalid robot index " << index << "." << std::endl;
        return;
    }

    setCommandUnlocked(index, pwmLeft, pwmRight, reverseLeft, reverseRight);
    rebuildPacket();
}

void CommunicationESPNow::sendMessage()
{
    std::lock_guard<std::mutex> lock(_commMutex);

    if(_serialFd == -1)
        return;

    _packetCounter++;
    rebuildPacket();
    writeAll(_packet.data(), _packet.size());
}

void CommunicationESPNow::stopAll()
{
    std::lock_guard<std::mutex> lock(_commMutex);

    for(int i = 0; i < ROBOT_COUNT; i++)
        setCommandUnlocked(i, 0, 0, false, false);

    rebuildPacket();
}

void CommunicationESPNow::getMessage()
{
    std::lock_guard<std::mutex> lock(_commMutex);

    for(int i = 0; i < PACKET_SIZE; i++)
        std::cout << std::hex << static_cast<unsigned short>(_packet[i]) << std::endl;
    std::cout << std::dec << std::endl << std::endl;
}

int CommunicationESPNow::getLeftPwm(int id)
{
    std::lock_guard<std::mutex> lock(_commMutex);

    if(id < 0 || id >= ROBOT_COUNT)
        return 0;
    return _commands[id].pwmLeft;
}

int CommunicationESPNow::getRightPwm(int id)
{
    std::lock_guard<std::mutex> lock(_commMutex);

    if(id < 0 || id >= ROBOT_COUNT)
        return 0;
    return _commands[id].pwmRight;
}
