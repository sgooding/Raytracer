#ifndef _Types_H
#define _Types_H

#include <math.h>

static inline unsigned char clampnround(float x)
{
    if (x > 255)
        x = 255;
    if (x < 0)
        x = 0;
    return (unsigned char)floor(x + .5);
}

/* ----------------------- */
struct RGB
{
    float r;
    float g; 
    float b;
};

struct Point2D
{
    int x;
    int y;
};

#endif