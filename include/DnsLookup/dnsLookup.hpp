#pragma once
#include <cctype>
#include <cstdarg>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "dnsTypes.hpp"
#include "udpInterface.hpp"
#include "helperFunctions.hpp"

class DnsLookup
{
public:
    DnsLookup() = default;
    ~DnsLookup() = default;

    // Add dns servers, can be entered in a row like ("1.1.1.1", "2.2.2.2", "3.3.3.3")
    template <typename... Args>
    void addDnsServer(Args &&...args)
    {
        (this->dnsServers.push_back(args), ...);
    }

    // Form a DNS query packet to be sent to the DNS server
    // dnsServiceName: The name of the service to be resolved e.g. "google.com"
    // dnsType: The type of DNS query to be made e.g. DnsType::A
    void formDnsQuery(std::string dnsServiceName, DnsType dnsType);

    // Send the DNS query to the DNS server and receive the response
    // UdpClass: The class that will be used to send the DNS query must be a subclass of IUDPInterface
    template <typename UdpClass>
    void sendAndReceiveDnsQuery()
    {
        std::unique_ptr<IUDPInterface> udpClient = std::make_unique<UdpClass>(std::string("53"));
        for (auto &dnsServer : dnsServers)
        {
            if (udpClient->begin())
            {
                udpClient->send(dnsPacket, dnsServer);

                this->dnsResponse = udpClient->receive();
                if (this->dnsResponse.size() > 0)
                    return;
            }
        }
    }

    // Parse the DNS response and return a vector of the records
    // RecordType: The type of record to be parsed e.g. A_Record can be manualy defined structure
    // dataParser: A function that will be used to parse the data (rdata) from the DNS response answer section
    template <typename RecordType>
    std::vector<RecordType> parseDnsResponse(std::function<RecordType(const DnsAnswer &, const HelperFunctions &)> dataParser)
    {
        std::vector<RecordType> records;

        std::vector<DnsAnswer> dnsAnswers = this->dnsParseStatic();
        for (auto &ans : dnsAnswers)
        {
            if (RecordType record; (DnsType)ans.answeredQuestion.qtype == record.type)
            {
                auto t = dataParser(ans, helperFunctions);

                // convert t to RecordType
                records.push_back(t);
            }
        }
        return records;
    }

private:
    HelperFunctions helperFunctions;

    std::vector<DnsAnswer> dnsParseStatic();

    std::vector<std::string> dnsServers;
    std::vector<uint8_t> dnsPacket;
    std::vector<uint8_t> dnsResponse;
};