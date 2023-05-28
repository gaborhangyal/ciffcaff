#include "ciff_parser.h"
#include "helper_functions.h"

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

    // Read caption and tags
    std::getline(stream, image.header.caption, '\n');

    std::string tag;
    while (static_cast<uint64_t>(stream.tellg()) < image.header.header_size)
    {
        std::getline(stream, tag, '\0');
        image.header.tags.push_back(tag);
    }

    // Read pixel data
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
