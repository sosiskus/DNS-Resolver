#pragma once
#include <string>
#include <vector>
#include <stdint.h>

class HelperFunctions
{
public:
    HelperFunctions() = default;
    ~HelperFunctions() = default;

    std::vector<uint8_t> readName(const std::vector<uint8_t> &dnsResponse, int pos, int &nextPos) const;
    std::vector<uint8_t> extractNameFromPointer(uint16_t namePointer, std::vector<uint8_t> &dnsResponse) const;
    std::vector<uint8_t> formatName(const std::string name) const;
};