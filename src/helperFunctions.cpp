#include "DnsLookup/helperFunctions.hpp"

std::vector<uint8_t> HelperFunctions::formatName(const std::string name) const
{
    std::vector<uint8_t> formattedName;
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

std::vector<uint8_t> HelperFunctions::extractNameFromPointer(uint16_t namePointer, std::vector<uint8_t> &dnsResponse) const
{
    namePointer = namePointer & 0x3FFF; // remove first 2 bits
    namePointer++;                      // skip first byte
    std::vector<uint8_t> name;

    // seek for first zero
    int end = 0;
    for (int i = namePointer; i < dnsResponse.size(); i++)
    {
        if (dnsResponse[i] == 0)
        {
            end = i;
            break;
        }
    }

    std::copy(dnsResponse.begin() + namePointer, dnsResponse.begin() + end, std::back_inserter(name));
    return name;
}

std::vector<uint8_t> HelperFunctions::readName(const std::vector<uint8_t> &dnsResponse, int pos, int &nextPos) const
{
    std::vector<uint8_t> name;
    int offset = 0;
    while (true)
    {
        uint8_t len = dnsResponse[pos + offset];
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