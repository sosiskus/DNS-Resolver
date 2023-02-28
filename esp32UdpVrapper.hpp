// #pragma once
// #include "udpInterface.hpp"
// #include "../UDP.hpp"

// class Esp32UdpWrapper : public IUDPInterface
// {
// private:
//     UDP_Connection udp;

// public:
//     Esp32UdpWrapper(std::string port) : IUDPInterface(port), udp(atoi(port.c_str()), 1024) {}
//     bool begin() override
//     {
//         this->udp.begin();
//         return true;
//     }
//     void send(const std::vector<unsigned char> &data, std::string ip) override
//     {
//         IPAddress host;
//         host.fromString(ip.c_str());
//         this->udp.send(data, host);
//     }
//     std::vector<unsigned char> receive() override
//     {
//         uchar_vector t;
//         while (udp.empty(t))
//         {
//             t = udp.recieve();
//         }
//         return t;
//     }

//     void close() override
//     {
//         udp.flush();
//     }
// };