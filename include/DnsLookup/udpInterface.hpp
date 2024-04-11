#pragma once
#include <string>
#include <vector>

class IUDPInterface
{
protected:
    std::string localPort;

public:
    IUDPInterface(std::string port) : localPort(port) {}
    virtual bool begin() = 0;
    virtual void send(const std::vector<uint8_t> &data, std::string ip) = 0;
    virtual std::vector<uint8_t> receive() = 0;

    virtual void close() = 0;
};