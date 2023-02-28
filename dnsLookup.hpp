#pragma once
#include <string>
#include <vector>
#include <cstdarg>
#include <cctype>
#include <ctime>
#include <memory>
#include <functional>

#include "udpInterface.hpp"
#include "dnsTypes.hpp"

class DnsLookup
{
private:
    std::vector<std::string> dnsServers;
    std::vector<unsigned char> dnsPacket;
    std::vector<unsigned char> dnsRepsonse;

    std::vector<unsigned char> formatName(const std::string name);

public:
    template <typename... Args>
    void addDnsServer(Args &&...args)
    {
        (this->dnsServers.push_back(args), ...);
    }

    void formDnsQuery(std::string dnsServiceName, DnsType dnsType);

    template <typename UdpClass>
    void sendDnsQuery()
    {
        std::unique_ptr<IUDPInterface> udpClient = std::make_unique<UdpClass>(std::string("53"));
        for (auto &dnsServer : dnsServers)
        {
            if (udpClient->begin())
            {
                udpClient->send(dnsPacket, dnsServer);
                std::vector<unsigned char> dnsResponse = udpClient->receive();
                if (dnsResponse.size() > 0)
                {
                    this->dnsRepsonse = dnsResponse;
                    return;
                }
            }
        }
    }

private:
    std::vector<DnsAnswer> dnsParseStatic();

public:
    std::vector<unsigned char> readName(std::vector<unsigned char> &dnsResponse, int pos, int &nextPos);

    template <typename RecordType>
    std::vector<RecordType> parseDnsResponse(std::function<RecordType(DnsAnswer, DnsLookup &)> dataParser)
    {
        std::vector<RecordType> records;

        std::vector<DnsAnswer> dnsAnswers = this->dnsParseStatic();
        for (auto &ans : dnsAnswers)
        {
            if (RecordType record; (DnsType)ans.answeredQuestion.qtype == record.type)
            {
                auto t = dataParser(ans, *this);

                // convert t to RecordType
                records.push_back(t);
            }
        }
        return records;
    }
};