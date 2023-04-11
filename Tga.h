#ifndef TGA_H
#define TGA_H

#include <stdio.h>
#include <vector>

#include "RgbColor.h"

class Tga {
    private:
        int height; // The height of the entire image.
        int width; // The width of the entire image.

    public:
        Tga(int height, int width);

        // Save as uncompressed tga
        bool save(RgbColor* colors, const char* filename);
};

#endif //TGA_H
