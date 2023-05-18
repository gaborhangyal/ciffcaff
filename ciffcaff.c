#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <jpeglib.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <filesystem>

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
}

CiffImage parseCiffFile(std::istream &stream)
{
    CiffImage image;

    // Read header
    seekToMagic(stream, "CIFF");

    stream.read(image.header.magic, 4);
    stream.read(reinterpret_cast<char *>(&image.header.header_size), 8);
    stream.read(reinterpret_cast<char *>(&image.header.content_size), 8);
    stream.read(reinterpret_cast<char *>(&image.header.width), 8);
    stream.read(reinterpret_cast<char *>(&image.header.height), 8);

    //std::cout << image.header.magic << std::endl;
    //std::cout << image.header.header_size << std::endl;
    //std::cout << image.header.content_size << std::endl;
    //std::cout << image.header.width << std::endl;
    //std::cout << image.header.height << std::endl;

    std::getline(stream, image.header.caption, '\n');

    std::string tag;
    while (static_cast<uint64_t>(stream.tellg()) < image.header.header_size)
    {
        std::getline(stream, tag, '\0');
        image.header.tags.push_back(tag);
    }

    RgbPixel pixel;
    while (stream.read(reinterpret_cast<char *>(&pixel), sizeof(RgbPixel)))
    {
        image.pixels.push_back(pixel);
    }

    return image;
}
void convertCiffToJpeg(const CiffImage &image, const std::string &outputPath)
{
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;

    // Setup JEG compression
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    // Open output file
    FILE *outfile = fopen(outputPath.c_str(), "wb");
    if (!outfile)
    {
        return;
    }

    jpeg_stdio_dest(&cinfo, outfile);

    // Set parameters for compression
    cinfo.image_width = image.header.width;
    cinfo.image_height = image.header.height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    std::vector<unsigned char> buffer(image.header.width * 3);

    // Write pixel data
    while (cinfo.next_scanline < cinfo.image_height)
    {
        for (uint64_t i = 0; i < image.header.width; i++)
        {
            buffer[i * 3 + 0] = image.pixels[cinfo.next_scanline * image.header.width + i].r;
            buffer[i * 3 + 1] = image.pixels[cinfo.next_scanline * image.header.width + i].g;
            buffer[i * 3 + 2] = image.pixels[cinfo.next_scanline * image.header.width + i].b;
        }
        unsigned char *row_pointer = &buffer[0];
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
}

// Function to parse a CAFF file
CaffFile parseCaffFile(std::istream &stream)
{
    CaffFile caff;

    seekToMagic(stream, "CAFF");
    // Read the header
    stream.read(caff.header.magic, 4);
    stream.read(reinterpret_cast<char *>(&caff.header.header_size), 8);
    stream.read(reinterpret_cast<char *>(&caff.header.num_ciffs), 8);
    std::cout << caff.header.magic << std::endl;
    std::cout << caff.header.header_size << std::endl;
    std::cout << caff.header.num_ciffs << std::endl;

    // Read the blocks
    while (!stream.eof())
    {
        // Read the block ID and length
        uint8_t block_id;
        uint64_t block_length;
        stream.read(reinterpret_cast<char *>(&block_id), 1);
        stream.read(reinterpret_cast<char *>(&block_length), 8);
        std::cout << "Block ID: " << (int)block_id << std::endl;
        std::cout << "Block length: " << block_length << std::endl;

        // Handle the block based on its ID
        // Header block already read so start with the block ID 2
        if (block_id == 2)
        {
            // Credits block
            CaffCredits credits;
            stream.read(reinterpret_cast<char *>(&credits.year), 2);
            stream.read(reinterpret_cast<char *>(&credits.month), 1);
            stream.read(reinterpret_cast<char *>(&credits.day), 1);
            stream.read(reinterpret_cast<char *>(&credits.hour), 1);
            stream.read(reinterpret_cast<char *>(&credits.minute), 1);
            stream.read(reinterpret_cast<char *>(&credits.creator_length), 8);
            credits.creator.resize(credits.creator_length);
            stream.read(&credits.creator[0], credits.creator_length);
            caff.credits = credits;
        }
        else if (block_id == 3)
        {
            // Animation block
            CaffAnimation animation;
            stream.read(reinterpret_cast<char *>(&animation.duration), 8);

            // Read the CIFF data
            uint64_t ciff_data_length = block_length - 8; // Subtract the length of the duration
            std::vector<char> ciff_data(ciff_data_length);
            stream.read(ciff_data.data(), ciff_data_length);

            // Create a string stream with the CIFF data
            std::istringstream ciff_data_stream(std::string(ciff_data.begin(), ciff_data.end()));

            // Parse the CIFF data
            animation.ciff = parseCiffFile(ciff_data_stream);
            caff.animations.push_back(animation);
            // break out of the loop as the hw specification says that only the first image should be converted
            break;
        }
        else
        {
            // Unknown block, skip it
            stream.seekg(block_length, std::ios::cur);
            std::cout << "Unknown block, skipping" << std::endl;
        }
    }

    return caff;
}

int main(int argc, char *argv[])
{
    // Check if the number of arguments is correct
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " –ciff [path-to-ciff].ciff" << std::endl;
        std::cerr << "or: " << argv[0] << " –caff [path-to-caff].caff" << std::endl;
        exit(-1);
    }

    // Check if we should parse a CIFF or a CAFF file
    if (strcmp(argv[1], "-ciff") == 0)
    {
        // Open the CIFF file
        std::ifstream file(argv[2], std::ios::binary);
        if (!file)
        {
            std::cerr << "Failed to open CIFF file: " << argv[2] << std::endl;
            exit(-1);
        }

        // Parse the CIFF file
        CiffImage image = parseCiffFile(file);

        // Close the file
        file.close();

        // Convert the CIFF image to JPEG
        std::string output_path = std::string(argv[2]) + ".jpg";
        convertCiffToJpeg(image, output_path);
    }
    else if (strcmp(argv[1], "-caff") == 0)
    {
        // Open the CAFF file
        std::ifstream file(argv[2], std::ios::binary);
        if (!file)
        {
            std::cerr << "Failed to open CAFF file: " << argv[2] << std::endl;
            exit(-1);
        }

        // Parse the CAFF file
        CaffFile caff = parseCaffFile(file);

        // Close the file
        file.close();

        std::string output_path = removeExtension(std::string(argv[2])) + ".jpg";

        convertCiffToJpeg(caff.animations[0].ciff, output_path);
    }
    else
    {
        std::cerr << "Unknown option: " << argv[1] << std::endl;
        exit(-1);
    }

    return 0;
}
