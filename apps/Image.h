#ifndef _Image_H
#define _Image_H

#include "Types.h"

#include <string>

class image
{
    int xsize, ysize; // resolution
    RGB *rgb;         // pixel intensities
public:
    image(int m, int n);      // allocates image of specified size
    RGB &pixel(int i, int j); // access to a specific pixel
    //void save_to_ppm_file ( char *filename );
    void save_to_ppm_file(std::string filename);
};
#endif