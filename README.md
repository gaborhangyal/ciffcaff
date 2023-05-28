# CiffCaff parser

This program is able to convert ciff files to jpg files, and it can extract the first ciff image file from caff files.

In order to compile this program, you need to have the following libraries installed:
- libjpeg

Usage:
```
Usage: ./parser –ciff [path-to-ciff].ciff
or:    ./parser –caff [path-to-caff].caff
```
The output file will be in the same directory as the input file, and it will have the same name as the input file, but with the jpg extension.