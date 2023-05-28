#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <string>
#include <istream>

// Function to remove the extension from a filename
std::string removeExtension(const std::string &filename);

// Function to seek to a magic string in a stream
void seekToMagic(std::istream &stream, const std::string &magic);

#endif
