#pragma once
#include <string>
#include <vector>
#include <stdint.h>

// Class that holds helper functions that are used to parse or format DNS data
class HelperFunctions
{
public:
    HelperFunctions() = default;
    ~HelperFunctions() = default;

    // Function that reads a string from the DNS response
    // dnsResponse: The DNS response raw data
    // pos: The position in the DNS response where the string starts
    // nextPos: Will be set to the position where the string ends
    // Returns: The string that was read in bytes array format
    std::vector<uint8_t> readName(const std::vector<uint8_t> &dnsResponse, int pos, int &nextPos) const;

    // Function that extracts a string from a pointer in the DNS response
    // namePointer: The pointer (2 bytes with first 2 bits 1) to the string in the DNS response raw data
    // dnsResponse: The DNS response raw data
    // Returns: The string that was read in bytes array format
    std::vector<uint8_t> extractNameFromPointer(uint16_t namePointer, std::vector<uint8_t> &dnsResponse) const;

    // Function that formats a string to be sent in the DNS query
    // name: The string to be formatted
    // Returns: The formatted string in bytes array format
    std::vector<uint8_t> formatName(const std::string name) const;
};