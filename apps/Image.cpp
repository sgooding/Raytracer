#include "Image.h"

#include <fstream>
#include <assert.h>

using namespace std;

/* ----------- image class: methods ---------- */

image::image(int m, int n) : xsize(m), ysize(n), rgb(NULL)
{
    rgb = new RGB[m * n];
}

/* ----------------------- */

RGB &image::pixel(int i, int j)
{
    return rgb[i + xsize * j];
}

/* ----------------------- */

void image::save_to_ppm_file(std::string filename)
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
