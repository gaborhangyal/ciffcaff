#ifndef CIFF_PARSER_H
#define CIFF_PARSER_H

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <jpeglib.h>

// Structure for CIFF header
struct CiffHeader
{
    char magic[4];
    uint64_t header_size;
    uint64_t content_size;
    uint64_t width;
    uint64_t height;
    std::string caption;
    std::vector<std::string> tags;
};

// Structure for RGB pixel
struct RgbPixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// Structure for CIFF image (header + pixel data)
struct CiffImage
{
    CiffHeader header;
    std::vector<RgbPixel> pixels;
};

// Function to parse a CIFF file
CiffImage parseCiffFile(std::istream &stream);

// Function to convert a CIFF image to JPEG
void convertCiffToJpeg(const CiffImage &image, const std::string &outputPath);

#endif
