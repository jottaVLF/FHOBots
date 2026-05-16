#ifndef COMMUNICATION_ESPNOW_HPP
#define COMMUNICATION_ESPNOW_HPP

#include "ICommunication.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>

class CommunicationESPNow : public ICommunication
{
    public:
        explicit CommunicationESPNow(const std::string &serialPort, int baudRate = 115200);
        ~CommunicationESPNow();

        void writeMessage(const int index, const int pwmLeft, const int pwmRight, const bool reverseLeft = false, const bool reverseRight = false);
        void sendMessage();
        void stopAll();
        void getMessage();

        int getLeftPwm(int id);
        int getRightPwm(int id);

    private:
        struct RobotCommand
        {
            uint8_t id;
            uint8_t pwmLeft;
            uint8_t pwmRight;
            uint8_t flags;
        };

        static constexpr uint8_t HEADER_0 = 0xA5;
        static constexpr uint8_t HEADER_1 = 0x5A;
        static constexpr int ROBOT_COUNT = 3;
        static constexpr int PACKET_SIZE = 18;

        uint8_t clampPwm(int pwm);
        uint8_t directionFlags(bool reverseLeft, bool reverseRight);
        uint8_t calculateChecksum();
        void rebuildPacket();
        void setCommandUnlocked(const int index, const int pwmLeft, const int pwmRight, const bool reverseLeft, const bool reverseRight);

        bool openSerialPort();
        bool configureSerialPort();
        bool writeAll(const uint8_t *data, std::size_t size);

        std::mutex _commMutex;
        std::string _serialPort;
        int _baudRate;
        int _serialFd;
        uint16_t _packetCounter;
        std::array<RobotCommand, ROBOT_COUNT> _commands;
        std::array<uint8_t, PACKET_SIZE> _packet;
};

#endif
