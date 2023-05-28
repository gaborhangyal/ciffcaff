#ifndef CAFF_PARSER_H
#define CAFF_PARSER_H

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "ciff_parser.h"

// Structure for CAFF header
struct CaffHeader
{
    char magic[4];
    uint64_t header_size;
    uint64_t num_ciffs;
};

// Structure for CAFF credits
struct CaffCredits
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint64_t creator_length;
    std::string creator;
};

// Structure for CAFF animation
struct CaffAnimation
{
    uint64_t duration;
    CiffImage ciff;
};

// Structure for CAFF file (header + credits + animations)
struct CaffFile
{
    CaffHeader header;
    CaffCredits credits;
    std::vector<CaffAnimation> animations;
};

// Function to parse a CAFF file
CaffFile parseCaffFile(std::istream &stream);

#endif
