#pragma once
#include <string>
#include <vector>
#include <functional>

struct DnsHeader
{
    unsigned short id;
    unsigned char rd : 1;
    unsigned char tc : 1;
    unsigned char aa : 1;
    unsigned char opcode : 4;
    unsigned char qr : 1;
    unsigned char rcode : 4;
    unsigned char cd : 1;
    unsigned char ad : 1;
    unsigned char z : 1;
    unsigned char ra : 1;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

struct DnsQuestion
{
    std::vector<unsigned char> qname;
    unsigned short qtype;
    unsigned short qclass;
};

enum class DnsType
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

struct DnsAnswer
{
    DnsQuestion answeredQuestion;
    unsigned int ttl;
    unsigned short rdlength;
    std::vector<unsigned char> rdata;
};