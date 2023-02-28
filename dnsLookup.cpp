#include "dnsLookup.hpp"

#include <string>
#include <vector>
#include <cstdarg>
#include <cctype>
#include <ctime>
#include <memory>
#include <functional>

// #include "Arduino.h"

std::vector<unsigned char> DnsLookup::formatName(const std::string name)
{
    std::vector<unsigned char> formattedName;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = name.find('.', prev)) != std::string::npos)
    {
        formattedName.push_back(pos - prev);
        for (std::string::size_type i = prev; i < pos; ++i)
        {
            formattedName.push_back(name[i]);
        }
        prev = pos + 1;
    }
    formattedName.push_back(name.size() - prev);
    for (std::string::size_type i = prev; i < name.size(); ++i)
    {
        formattedName.push_back(name[i]);
    }
    formattedName.push_back(0);
    return formattedName;
}

void DnsLookup::formDnsQuery(std::string dnsServiceName, DnsType dnsType)
{
    
    // Generate random id
    srand(time(NULL));

    // Define dns header
    DnsHeader dnsHeader;
    dnsHeader.id = (unsigned short)(rand() % 65536);
    dnsHeader.qr = 0;     // This is a query
    dnsHeader.opcode = 0; // This is a standard query
    dnsHeader.aa = 0;     // Not Authoritative
    dnsHeader.tc = 0;     // This message is not truncated
    dnsHeader.rd = 1;     // Recursion Desired
    dnsHeader.ra = 0;     // Recursion not available! hey we dont have it (lol)
    dnsHeader.z = 0;
    dnsHeader.ad = 0;
    dnsHeader.cd = 0;
    dnsHeader.rcode = 0;
    dnsHeader.qdcount = 1; // we have only 1 question
    dnsHeader.ancount = 0;
    dnsHeader.nscount = 0;
    dnsHeader.arcount = 0;

    // Form dns question
    DnsQuestion dnsQuestion;
    dnsQuestion.qname = formatName(dnsServiceName);

    dnsQuestion.qtype = (unsigned short)dnsType;
    dnsQuestion.qclass = 1;

    std::vector<unsigned char> dnsQuery;
    dnsQuery.push_back((dnsHeader.id >> 8) & 255);
    dnsQuery.push_back(dnsHeader.id & 255);
    dnsQuery.push_back((dnsHeader.qr << 7) | (dnsHeader.opcode << 3) | (dnsHeader.aa << 2) | (dnsHeader.tc << 1) | dnsHeader.rd);
    dnsQuery.push_back((dnsHeader.ra << 7) | (dnsHeader.z << 6) | (dnsHeader.ad << 5) | (dnsHeader.cd << 4) | dnsHeader.rcode);
    dnsQuery.push_back((dnsHeader.qdcount >> 8) & 255);
    dnsQuery.push_back(dnsHeader.qdcount & 255);
    dnsQuery.push_back((dnsHeader.ancount >> 8) & 255);
    dnsQuery.push_back(dnsHeader.ancount & 255);
    dnsQuery.push_back((dnsHeader.nscount >> 8) & 255);
    dnsQuery.push_back(dnsHeader.nscount & 255);
    dnsQuery.push_back((dnsHeader.arcount >> 8) & 255);
    dnsQuery.push_back(dnsHeader.arcount & 255);

    for (auto &c : dnsQuestion.qname)
    {
        dnsQuery.push_back(c);
    }

    dnsQuery.push_back((dnsQuestion.qtype >> 8) & 255);
    dnsQuery.push_back(dnsQuestion.qtype & 255);
    dnsQuery.push_back((dnsQuestion.qclass >> 8) & 255);
    dnsQuery.push_back(dnsQuestion.qclass & 255);

    this->dnsPacket = dnsQuery;

    
    for (auto &c : dnsQuery)
    {
        
    }
    
}

std::vector<DnsAnswer> DnsLookup::dnsParseStatic()
{
    DnsHeader dnsHeader;
    int pos = 0;
    dnsHeader.id = (dnsRepsonse[pos] << 8) + dnsRepsonse[pos + 1];
    pos += 2;
    dnsHeader.qr = (dnsRepsonse[pos] >> 7) & 1;
    dnsHeader.opcode = (dnsRepsonse[pos] >> 3) & 15;
    dnsHeader.aa = (dnsRepsonse[pos] >> 2) & 1;
    dnsHeader.tc = (dnsRepsonse[pos] >> 1) & 1;
    dnsHeader.rd = dnsRepsonse[pos] & 1;
    pos++;
    dnsHeader.ra = (dnsRepsonse[pos] >> 7) & 1;
    dnsHeader.z = (dnsRepsonse[pos] >> 4) & 7;
    dnsHeader.rcode = dnsRepsonse[pos] & 15;
    pos++;
    dnsHeader.qdcount = (dnsRepsonse[pos] << 8) + dnsRepsonse[pos + 1];
    pos += 2;
    dnsHeader.ancount = (dnsRepsonse[pos] << 8) + dnsRepsonse[pos + 1];
    pos += 2;
    dnsHeader.nscount = (dnsRepsonse[pos] << 8) + dnsRepsonse[pos + 1];
    pos += 2;
    dnsHeader.arcount = (dnsRepsonse[pos] << 8) + dnsRepsonse[pos + 1];
    pos += 2;

    // Recieved DNS questions
    std::vector<DnsQuestion> dnsQuestions;
    for (int i = 0; i < dnsHeader.qdcount; i++)
    {
        DnsQuestion dnsQuestion;
        int nextPos;
        dnsQuestion.qname = readName(dnsRepsonse, pos, nextPos);
        pos = nextPos;
        dnsQuestion.qtype = (dnsRepsonse[pos] << 8) + dnsRepsonse[pos + 1];
        pos += 2;
        dnsQuestion.qclass = (dnsRepsonse[pos] << 8) + dnsRepsonse[pos + 1];
        pos += 2;
        dnsQuestions.push_back(dnsQuestion);
    }

    // Recieved DNS answers
    std::vector<DnsAnswer> dnsAnswers;
    for (int i = 0; i < dnsHeader.ancount; i++)
    {
        DnsAnswer dnsAnswer;
        // int nextPos;
        dnsAnswer.answeredQuestion.qname = dnsQuestions[i].qname;
        pos += 2; // Pointer size
        dnsAnswer.answeredQuestion.qtype = (dnsRepsonse[pos] << 8) + dnsRepsonse[pos + 1];
        pos += 2;
        dnsAnswer.answeredQuestion.qclass = (dnsRepsonse[pos] << 8) + dnsRepsonse[pos + 1];
        pos += 2;
        dnsAnswer.ttl = (dnsRepsonse[pos] << 24) + (dnsRepsonse[pos + 1] << 16) + (dnsRepsonse[pos + 2] << 8) + dnsRepsonse[pos + 3];
        pos += 4;
        dnsAnswer.rdlength = (dnsRepsonse[pos] << 8) + dnsRepsonse[pos + 1];
        pos += 2;
        for (int j = 0; j < dnsAnswer.rdlength; j++)
        {
            dnsAnswer.rdata.push_back(dnsRepsonse[pos + j]);
        }
        dnsAnswers.push_back(dnsAnswer);
    }
    return dnsAnswers;
}

std::vector<unsigned char> DnsLookup::readName(std::vector<unsigned char> &dnsResponse, int pos, int &nextPos)
{
    std::vector<unsigned char> name;
    int offset = 0;
    while (true)
    {
        unsigned char len = dnsResponse[pos + offset];
        if (len == 0)
        {
            nextPos = pos + offset + 1;
            break;
        }
        else
        {
            for (int i = 0; i < len; i++)
            {
                name.push_back(dnsResponse[pos + offset + 1 + i]);
            }
            if (dnsResponse[pos + offset + 1 + len] == 0x00)
            {
                nextPos = pos + offset + 1 + len + 1;
                break;
            }
            else
            {
                name.push_back('.');
                offset += len + 1;
            }
        }
    }
    return name;
}