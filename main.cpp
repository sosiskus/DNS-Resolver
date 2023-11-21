#include <iostream>
#include "dnsLookup.hpp"
#include "sockets.hpp"
#include "defaultParsers.hpp"

int main()
{
    DnsLookup dnsLookup;
    dnsLookup.addDnsServer("1.1.1.1");
    dnsLookup.formDnsQuery("ota_port.fovibalt.com", DnsType::TXT);

    dnsLookup.sendDnsQuery<UDPInterface>();
    std::cout << "DNS Response: " << std::endl;

    struct TXTRecord
    {
        const DnsType type = DnsType::TXT;
        std::string txt;
    };

    std::function<TXTRecord(DnsAnswer, DnsLookup &)> TXT_RecordParser = [](DnsAnswer ans, DnsLookup &dns)
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
        std::cout << ans.txt << " " << (int)ans.txt.at(ans.txt.size()-2) << std::endl;
    }

    return 0;
}