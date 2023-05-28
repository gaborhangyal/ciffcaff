#include <iostream>
#include <cstring>

#include "caff_parser.h"
#include "helper_functions.h"

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
        // Remove the extension from the CIFF file path and add the JPEG extension
        std::string output_path = removeExtension(std::string(argv[2])) + ".jpg";
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

        // Remove the extension from the CAFF file path and add the JPEG extension
        std::string output_path = removeExtension(std::string(argv[2])) + ".jpg";

        // Convert the first CIFF image to JPEG
        convertCiffToJpeg(caff.animations[0].ciff, output_path);
    }
    else
    {
        std::cerr << "Unknown option: " << argv[1] << std::endl;
        exit(-1);
    }

    return 0;
}