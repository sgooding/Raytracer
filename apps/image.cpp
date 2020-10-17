#include "image.h"

#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;

/* ----------- image class: methods ---------- */

image::image(const int& m, const int& n) : 
            xsize(m), 
            ysize(n), 
            rgb(NULL)
{
    total = m * n;
    rgb = new RGB[total];
}

/* ----------------------- */

RGB &image::pixel(int i, int j)
{
    return rgb[i + xsize * j];
}

/* ----------------------- */

unsigned char image::clampnround(float x)
{
    if (x > 255) {
        x = 255;
    }
    if (x < 0) {
        x = 0;
    }
    return (unsigned char)floor(x + .5);
}

/* ----------------------- */

void image::save_to_ppm_file(const std::string& filename)
{
    ofstream ofs(filename.c_str(), ios::binary);
    assert(ofs);
    ofs << "P6" << endl;
    ofs << xsize << " " << ysize << endl
        << 255 << endl;

    unsigned char min_rgb[3] = {255, 255, 255};
    unsigned char max_rgb[3] = {0, 0, 0};

    for (int i = 0; i < xsize * ysize; i++)
    {
        unsigned char r = clampnround(256 * rgb[i].r);
        unsigned char g = clampnround(256 * rgb[i].g);
        unsigned char b = clampnround(256 * rgb[i].b);

        min_rgb[0] = std::min(min_rgb[0], r);
        min_rgb[1] = std::min(min_rgb[1], g);
        min_rgb[2] = std::min(min_rgb[2], b);

        max_rgb[0] = std::max(max_rgb[0], r);
        max_rgb[1] = std::max(max_rgb[1], g);
        max_rgb[2] = std::max(max_rgb[2], b);
    }

    float r_scale = 1.0;
    float g_scale = 1.0;
    float b_scale = 1.0;
    for (int i = 0; i < xsize * ysize; i++)
    {
        unsigned char r = clampnround(256 * rgb[i].r * r_scale);
        unsigned char g = clampnround(256 * rgb[i].g * g_scale);
        unsigned char b = clampnround(256 * rgb[i].b * b_scale);
        ofs.write((char *)&r, sizeof(char));
        ofs.write((char *)&g, sizeof(char));
        ofs.write((char *)&b, sizeof(char));
    }
}