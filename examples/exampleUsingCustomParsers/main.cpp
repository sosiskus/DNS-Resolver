#include "DnsLookup/defaultParsers.hpp"
#include "DnsLookup/dnsLookup.hpp"
#include "UdpWrapper/winSockets.hpp"
#include <iostream>

int main()
{
    DnsLookup dnsLookup;
    dnsLookup.addDnsServer("1.1.1.1");
    dnsLookup.formDnsQuery("ota_port.fovibalt.com", DnsType::TXT);

    dnsLookup.sendAndReceiveDnsQuery<WinSocketUDPInterface>();
    std::cout << "DNS Response: " << std::endl;

    struct TXTRecord
    {
        const DnsType type = DnsType::TXT;
        std::string txt;
    };

    std::function<TXTRecord(const DnsAnswer &, const HelperFunctions &)> TXT_RecordParser =
        [](const DnsAnswer &ans, const HelperFunctions &functions)
    {
        // Read first 2 bytes to get length of txt
        int txtLength = ans.rdata[0];
        TXTRecord txt;
        for (int i = 1; i < txtLength + 2; i++)
        {
            txt.txt += (char)ans.rdata[i];
        }

        return txt;
    };

    auto answers = dnsLookup.parseDnsResponse<TXTRecord>(TXT_RecordParser);

    for (auto &ans : answers)
    {
        std::cout << "TXT: " << ans.txt << std::endl;
    }

    return 0;
}