#pragma once
#include <string>
#include <vector>
#include <functional>
#include <stdint.h>

// Structure that holds the DNS header
struct DnsHeader
{
    uint16_t id;
    uint8_t rd : 1;
    uint8_t tc : 1;
    uint8_t aa : 1;
    uint8_t opcode : 4;
    uint8_t qr : 1;
    uint8_t rcode : 4;
    uint8_t cd : 1;
    uint8_t ad : 1;
    uint8_t z : 1;
    uint8_t ra : 1;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

// Structure that holds a DNS question
struct DnsQuestion
{
    std::vector<uint8_t> qname;
    uint16_t qtype;
    uint16_t qclass;
};

// Structure that holds a DNS answer
struct DnsAnswer
{
    DnsQuestion answeredQuestion;
    unsigned int ttl;
    uint16_t rdlength;
    std::vector<uint8_t> rdata;
};

// Enum class that holds the DNS types
enum class DnsType : uint8_t
{
    A = 1,
    NS = 2,
    MD = 3,
    MF = 4,
    CNAME = 5,
    SOA = 6,
    MB = 7,
    MG = 8,
    WKS = 11,
    PTR = 12,
    HINFO = 13,
    MINFO = 14,
    MX = 15,
    TXT = 16,
    AAAA = 28,
    SRV = 33,
    ANY = 255
};