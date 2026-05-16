#ifndef FAKE_COMMUNICATION_HPP
#define FAKE_COMMUNICATION_HPP

#include "ICommunication.hpp"
#include <array>
#include <iostream>

class FakeCommunication : public ICommunication {
public:
    FakeCommunication() {
        _pwmLeft.fill(0);
        _pwmRight.fill(0);
    }

    void writeMessage(const int index, const int pwmLeft, const int pwmRight,
                      const bool reverseLeft = false, const bool reverseRight = false) override {
        if (index >= 0 && index < 3) {
            _pwmLeft[index]  = reverseLeft  ? -pwmLeft  : pwmLeft;
            _pwmRight[index] = reverseRight ? -pwmRight : pwmRight;
        }
    }

    void sendMessage() override {}
    void stopAll() override { _pwmLeft.fill(0); _pwmRight.fill(0); }
    void getMessage() override {}

    int getLeftPwm(int id) override  { return (id >= 0 && id < 3) ? _pwmLeft[id]  : 0; }
    int getRightPwm(int id) override { return (id >= 0 && id < 3) ? _pwmRight[id] : 0; }

private:
    std::array<int, 3> _pwmLeft;
    std::array<int, 3> _pwmRight;
};

#endif
