#include "DnsLookup/helperFunctions.hpp"

#include <iostream>
#include <bits/stdc++.h>
std::vector<uint8_t> HelperFunctions::formatName(const std::string name) const
{
    std::vector<uint8_t> formattedName;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;

    while ((pos = name.find('.', prev)) != std::string::npos)
    {
        formattedName.push_back(pos - prev);
        std::copy(name.begin() + prev, name.begin() + pos, std::back_inserter(formattedName));
        prev = pos + 1;
    }

    formattedName.push_back(name.size() - prev);
    std::copy(name.begin() + prev, name.end(), std::back_inserter(formattedName));
    formattedName.push_back(0);

    return formattedName;
}

std::vector<uint8_t> HelperFunctions::extractNameFromPointer(uint16_t namePointer, std::vector<uint8_t> &dnsResponse) const
{
    namePointer = namePointer & 0x3FFF; // remove first 2 bits
    std::vector<uint8_t> name;

    // seek for first zero
    auto itterator = std::find(dnsResponse.begin() + namePointer, dnsResponse.end(), 0);
    if (itterator == dnsResponse.end())
    {
        std::cerr << "Error: Name pointer points to invalid location" << std::endl;
        return name;
    }

    std::copy(dnsResponse.begin() + namePointer, itterator, std::back_inserter(name));
    
    return name;
}

std::vector<uint8_t> HelperFunctions::readName(const std::vector<uint8_t> &dnsResponse, int pos, int &nextPos) const
{
    std::vector<uint8_t> name;

    // read first byte this is the length of the first part of the name, read all parts until 0x00
    while (pos < dnsResponse.size())
    {
        uint8_t len = dnsResponse[pos];
        if (len == 0)
        {
            nextPos = pos + 1;
            break;
        }
        else
        {
            std::copy(dnsResponse.begin() + pos + 1, dnsResponse.begin() + pos + 1 + len, std::back_inserter(name));
            name.push_back('.');
            pos += len + 1;
        }
    }

    name.pop_back(); // remove last dot
    return name;
}