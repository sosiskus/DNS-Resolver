#pragma once

#include "dnsTypes.hpp"
#include "dnsLookup.hpp"

struct A_Record
{
    const static DnsType type = DnsType::A;
    unsigned char ip[4];
};

struct SRV_Record
{
    const static DnsType type = DnsType::SRV;
    unsigned short priority;
    unsigned short weight;
    unsigned short port;
    std::string target;
};

std::function<A_Record(DnsAnswer, DnsLookup &)> A_RecordParser = [](DnsAnswer ans, DnsLookup &dns)
{ return *(A_Record *)ans.rdata.data(); };

std::function<SRV_Record(DnsAnswer, DnsLookup &)> SRV_RecordParser = [](DnsAnswer ans, DnsLookup &dns)
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
    auto target = dns.readName(ans.rdata, pos, nextPos);
    for (int j = 0; j < target.size(); j++)
    {
        srv.target += (char)target[j];
    }
    return srv;
};