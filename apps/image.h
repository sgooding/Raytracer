
#ifndef _image_h
#define _image_h

#include <string>

/* ------------ typedefs -------------- */

typedef struct
{
    float r, g, b;
} RGB;

class image
{
public:

    image(const int& m, const int& n);      // allocates image of specified size

    RGB &pixel(int i, int j); // access to a specific pixel

    void save_to_ppm_file(const std::string& filename);

    unsigned char clampnround(float x);

    int xsize, ysize; // resolution

    int total;        // total number of pixels

private:
    RGB *rgb;         // pixel intensities
};


#endif