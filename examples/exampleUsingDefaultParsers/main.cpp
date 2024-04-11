#include "DnsLookup/defaultParsers.hpp"
#include "DnsLookup/dnsLookup.hpp"
#include "UdpWrapper/winSockets.hpp"
#include <iostream>

int main()
{
    DnsLookup dnsLookup;
    dnsLookup.addDnsServer("1.1.1.1", "8.8.8.8", "1.0.0.1");
    dnsLookup.formDnsQuery("www.fovibalt.com", DnsType::A);

    dnsLookup.sendAndReceiveDnsQuery<WinSocketUDPInterface>();
    std::cout << "DNS Response: " << std::endl;

    auto answers = dnsLookup.parseDnsResponse<A_Record>(A_RecordParser);

    for (auto &ans : answers)
    {
        std::cout << (int)ans.ip[0] << " " << (int)ans.ip[1] << " " << (int)ans.ip[2] << " " << (int)ans.ip[3] << std::endl;
    }

    return 0;
}