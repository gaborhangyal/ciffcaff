#include "caff_parser.h"
#include "helper_functions.h"

CaffFile parseCaffFile(std::istream &stream)
{
    CaffFile caff;

    seekToMagic(stream, "CAFF");
    // Read the header
    stream.read(caff.header.magic, 4);
    stream.read(reinterpret_cast<char *>(&caff.header.header_size), 8);
    stream.read(reinterpret_cast<char *>(&caff.header.num_ciffs), 8);

    // Read the blocks
    while (!stream.eof())
    {
        // Read the block ID and length
        uint8_t block_id;
        uint64_t block_length;
        stream.read(reinterpret_cast<char *>(&block_id), 1);
        stream.read(reinterpret_cast<char *>(&block_length), 8);

        // Handle the block based on its ID

        if (block_id == 1)
        {
            // Skip second header block as the first one should be used
            stream.seekg(block_length, std::ios::cur);
        }

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