#pragma once
#include "DnsLookup/udpInterface.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

// WinSocket implementation of the UDP interface
class WinSocketUDPInterface : public IUDPInterface
{
private:
    SOCKET sockfd; // socket file descriptor

public:
    WinSocketUDPInterface(std::string port) : IUDPInterface(port) {}

    bool begin() override
    {
        // Initialize Winsock
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0)
        {
            std::cerr << "WSAStartup failed: " << iResult << std::endl;
            return false;
        }

        // Create socket
        sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sockfd == INVALID_SOCKET)
        {
            std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
            return false;
        }

        return true;
    }

    void send(const std::vector<uint8_t> &data, std::string ip) override
    {
        // Set up destination address and port
        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(std::stoi(localPort));
        dest_addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

        // Send data
        int sent_bytes = sendto(sockfd, (const char *)&data[0], data.size(), 0, (SOCKADDR *)&dest_addr, sizeof(dest_addr));
        if (sent_bytes == SOCKET_ERROR)
        {
            std::cerr << "Error sending packet: " << WSAGetLastError() << std::endl;
        }
        std::cout << "Sent " << sent_bytes << " bytes to " << ip << std::endl;
    }

    std::vector<uint8_t> receive() override
    {
        // Create buffer for received data
        std::vector<uint8_t> buffer(1024);

        // set timeout for recv
        fd_set fds;
        int n;
        struct timeval tv;

        // Set up the file descriptor set.
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);

        // Set up the struct timeval for the timeout.
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        // Wait until timeout or data received.
        n = select(sockfd, &fds, NULL, NULL, &tv);
        if (n == 0)
        {
            std::cout << "Timeout..." << std::endl;
            return {};
        }
        else if (n == -1)
        {
            std::cout << "Error..." << std::endl;
            return {};
        }

        // Receive data
        int received_bytes = recv(sockfd, (char *)&buffer[0], buffer.size(), 0);
        if (received_bytes == SOCKET_ERROR)
        {
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK)
            {
                std::cerr << "Error receiving packet: " << error << std::endl;
            }
            buffer.clear();
        }
        else
        {
            // Trim buffer to actual size
            buffer.resize(received_bytes);
        }

        return buffer;
    }

    void close() override
    {
        closesocket(sockfd);
        WSACleanup();
    }
};
