#pragma once

#include "dnsTypes.hpp"
#include "dnsLookup.hpp"

struct A_Record
{
    const static DnsType type = DnsType::A;
    uint8_t ip[4];
};

struct SRV_Record
{
    const static DnsType type = DnsType::SRV;
    uint16_t priority;
    uint16_t weight;
    uint16_t port;
    std::string target;
};

std::function<A_Record(const DnsAnswer &, const HelperFunctions &)> A_RecordParser = [](const DnsAnswer &ans, const HelperFunctions &functions)
{ return *(A_Record *)ans.rdata.data(); };

std::function<SRV_Record(const DnsAnswer &, const HelperFunctions &)> SRV_RecordParser = [](const DnsAnswer &ans, const HelperFunctions &functions)
{
    SRV_Record srv;
    int pos = 0;
    srv.priority = (ans.rdata[pos] << 8) + ans.rdata[pos + 1];
    pos += 2;
    srv.weight = (ans.rdata[pos] << 8) + ans.rdata[pos + 1];
    pos += 2;
    srv.port = (ans.rdata[pos] << 8) + ans.rdata[pos + 1];
    pos += 2;
    int nextPos;
    auto target = functions.readName(ans.rdata, pos, nextPos);
    for (int j = 0; j < target.size(); j++)
    {
        srv.target += (char)target[j];
    }
    return srv;
};