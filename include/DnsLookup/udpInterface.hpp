#pragma once
#include <string>
#include <vector>

// Interface for the UDP communication
class IUDPInterface
{
protected:
    std::string localPort;

public:
    IUDPInterface(std::string localPort) : localPort(localPort) {}
    virtual ~IUDPInterface() = default;

    virtual bool begin() = 0; // Initialize the UDP communication
    virtual void close() = 0; // Close the UDP communication

    // Send data to the specified IP address
    virtual void send(const std::vector<uint8_t> &data, std::string ip) = 0; 
    // Receive data
    virtual std::vector<uint8_t> receive() = 0;
};