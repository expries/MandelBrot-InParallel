#include <iostream>
#include <fstream>
#include "Tga.h"

Tga::Tga(int height, int width) {
    this->height = height;
    this->width = width;
}

bool Tga::save(RgbColor* colors, const char* filename)
{
	std::ofstream tgaFile;
    tgaFile.open (filename, std::ios::out | std::ios::trunc | std::ios::binary);

    // Create the tga header
    uint8_t header[18] = {0};
    header[2] = 2;  // uncompressed RGB
    header[12] = this->width & 0xff;
    header[13] = (this->width >> 8) & 0xff;
    header[14] = this->height & 0xff;
    header[15] = (this->height >> 8) & 0xff;
    header[16] = 24; // 24 bits per pixel

    // Write header
    tgaFile.write((char *) header, 18);

    // Write content from bottom to top, from left to right
    for (int x = 0; x < this->width; x++) {
        for (int y = 0; y < this->height; y++) {
            auto index = y * this->width + x;
            auto rgb = colors[index];
            unsigned char color[3] = {rgb.blue, rgb.green, rgb.red};
            tgaFile.write((char *) color, 3);
        }
    }

    tgaFile.close();
	return true;
}
