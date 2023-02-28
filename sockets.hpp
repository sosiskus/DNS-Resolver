#pragma once
#include "udpInterface.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <ws2tcpip.h>
#include <winsock2.h>


// #pragma comment(lib, "Ws2_32.lib")

class UDPInterface : public IUDPInterface
{
private:
    SOCKET sockfd;
    struct sockaddr_in addr;

public:
    UDPInterface(std::string port) : IUDPInterface(port) {}

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

        // Bind socket to local port
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(std::stoi(localPort));

        if (bind(sockfd, (SOCKADDR *)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
            closesocket(sockfd);
            return false;
        }

        return true;
    }

    void send(const std::vector<unsigned char> &data, std::string ip) override
    {
        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(std::stoi(localPort));
        // InetPtonW(AF_INET, ip.c_str(), &dest_addr.sin_addr);
        // std::wstring widestr = std::wstring(ip.begin(), ip.end());
        // const wchar_t* widecstr = widestr.c_str();
        // InetPton(AF_INET,ip.c_str(), &dest_addr.sin_addr);
        dest_addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

        int sent_bytes = sendto(sockfd, (const char *)&data[0], data.size(), 0, (SOCKADDR *)&dest_addr, sizeof(dest_addr));
        if (sent_bytes == SOCKET_ERROR)
        {
            std::cerr << "Error sending packet: " << WSAGetLastError() << std::endl;
        }
        std::cout << "Sent " << sent_bytes << " bytes to " << ip << std::endl;
    }

    std::vector<unsigned char> receive() override
    {
        std::vector<unsigned char> buffer(1024);
        struct sockaddr_in cli_addr;
        int clilen = sizeof(cli_addr);

        int received_bytes = recvfrom(sockfd, (char *)&buffer[0], buffer.size(), 0, (SOCKADDR *)&cli_addr, &clilen);
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
