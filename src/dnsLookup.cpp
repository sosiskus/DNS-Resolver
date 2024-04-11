#include "DnsLookup/dnsLookup.hpp"

#include <functional>
#include <memory>
#include <random>
#include <string>
#include <vector>

void DnsLookup::formDnsQuery(const std::string &dnsServiceName, const DnsType dnsType)
{
    // Generate random id
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<uint16_t> uniform_dist(0, std::numeric_limits<uint16_t>::max());

    // Define dns header
    DnsHeader dnsHeader;
    dnsHeader.id = uniform_dist(e1);
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
    dnsQuestion.qname = helperFunctions.formatName(dnsServiceName); // Convert domain name to dns format

    dnsQuestion.qtype = (uint16_t)dnsType; // Type of the query
    dnsQuestion.qclass = 1;                // This is internet class

    // Form the dns query packet
    std::vector<uint8_t> dnsQuery;
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

    std::copy(dnsQuestion.qname.begin(), dnsQuestion.qname.end(), std::back_inserter(dnsQuery));

    dnsQuery.push_back((dnsQuestion.qtype >> 8) & 255);
    dnsQuery.push_back(dnsQuestion.qtype & 255);
    dnsQuery.push_back((dnsQuestion.qclass >> 8) & 255);
    dnsQuery.push_back(dnsQuestion.qclass & 255);

    this->dnsPacket = dnsQuery;
}

std::vector<DnsAnswer> DnsLookup::dnsParseStatic()
{
    // Parse DNS header
    DnsHeader dnsHeader;
    int pos = 0;
    dnsHeader.id = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
    pos += 2;
    dnsHeader.qr = (dnsResponse[pos] >> 7) & 1;
    dnsHeader.opcode = (dnsResponse[pos] >> 3) & 15;
    dnsHeader.aa = (dnsResponse[pos] >> 2) & 1;
    dnsHeader.tc = (dnsResponse[pos] >> 1) & 1;
    dnsHeader.rd = dnsResponse[pos] & 1;
    pos++;
    dnsHeader.ra = (dnsResponse[pos] >> 7) & 1;
    dnsHeader.z = (dnsResponse[pos] >> 4) & 7;
    dnsHeader.rcode = dnsResponse[pos] & 15;
    pos++;
    dnsHeader.qdcount = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
    pos += 2;
    dnsHeader.ancount = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
    pos += 2;
    dnsHeader.nscount = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
    pos += 2;
    dnsHeader.arcount = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
    pos += 2;

    // Parse recieved DNS questions
    std::vector<DnsQuestion> dnsQuestions(dnsHeader.qdcount);
    for (int i = 0; i < dnsHeader.qdcount; i++)
    {
        DnsQuestion dnsQuestion;
        int nextPos = 0;
        dnsQuestion.qname = helperFunctions.readName(dnsResponse, pos, nextPos);
        pos = nextPos;
        dnsQuestion.qtype = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
        pos += 2;
        dnsQuestion.qclass = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
        pos += 2;
        dnsQuestions[i] = dnsQuestion;
    }

    // Parse recieved DNS answers
    std::vector<DnsAnswer> dnsAnswers(dnsHeader.ancount);
    for (int i = 0; i < dnsHeader.ancount; i++)
    {
        DnsAnswer dnsAnswer;

        uint16_t namePointer = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
        dnsAnswer.answeredQuestion.qname = helperFunctions.extractNameFromPointer(namePointer, dnsResponse);

        pos += 2; // Pointer size
        dnsAnswer.answeredQuestion.qtype = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
        pos += 2;
        dnsAnswer.answeredQuestion.qclass = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
        pos += 2;
        dnsAnswer.ttl = (dnsResponse[pos] << 24) + (dnsResponse[pos + 1] << 16) + (dnsResponse[pos + 2] << 8) + dnsResponse[pos + 3];
        pos += 4;
        dnsAnswer.rdlength = (dnsResponse[pos] << 8) + dnsResponse[pos + 1];
        pos += 2;

        // Copy rdata
        dnsAnswer.rdata.resize(dnsAnswer.rdlength);
        std::copy(dnsResponse.begin() + pos, dnsResponse.begin() + pos + dnsAnswer.rdlength, dnsAnswer.rdata.begin());

        pos += dnsAnswer.rdlength;

        dnsAnswers[i] = dnsAnswer; // Add the answer to the DNS answer struct vector
    }

    return dnsAnswers;
}