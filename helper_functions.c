#include "helper_functions.h"
#include <iostream>

std::string removeExtension(const std::string &filename)
{
    size_t lastDotPos = filename.find_last_of(".");
    if (lastDotPos != std::string::npos)
    {
        return filename.substr(0, lastDotPos);
    }
    return filename;
}

void seekToMagic(std::istream &stream, const std::string &magic)
{
    char c;
    std::string read_string;
    while (stream.get(c))
    {
        read_string += c;
        if (read_string.size() > magic.size())
        {
            read_string.erase(read_string.begin());
        }
        if (read_string == magic)
        {
            stream.seekg(-magic.size(), std::ios::cur);
            break;
        }
    }
    if (read_string != magic)
    {
        std::cerr << "Magic not found, exiting..." << magic << std::endl;
        exit(-1);
    }
}