
/*
   Sean Gooding
   Project 1: Ray Tracing
   CSCI 441
 */

#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>
#include "Vector.h"
#include "Primitives.h"
#include "RayTrace.h"
#include "RayTraceConfig.h"
#include "Image.h"
#include <vector>
#include <stdlib.h>

using namespace std;

/* ----------- Reading the input file ---------- */

// global variables
int resolution_x, resolution_y;
std::vector<smg::primitive *> gpPrimitives;
smg::RayTrace gRayTrace;
RayTraceConfig gRayTraceConfig;

/* ----------- function prototypes ---------- */
void Once(float x, float y, float &R, float &G, float &B);

void emit_photons(void);

/* ----------- main function ---------- */
int main(int argc, char *argv[])
{
    std::string filename("input.txt");
    if (argc == 2)
    {
        filename = argv[1];
        gRayTrace.AliasSize(2.0);
    }
    else if (argc == 3)
    {
        filename = argv[1];
        gRayTrace.AliasSize(float(atoi(argv[2])));
    }
    else
    {
        gRayTrace.AliasSize(2.0);
    }

    int x, y;

    gRayTraceConfig.read_input_file(filename, gRayTrace, gpPrimitives, resolution_x, resolution_y);

    if (gRayTrace.GetEnablePhotonMapper())
    {
        gRayTrace.EmitPhotons();
    }

    std::cout << "Begin Rendering" << std::endl;

    image img(resolution_x, resolution_y);
    float current(0.0);
    float total(resolution_x * resolution_y);
    for (x = 0; x < resolution_x; x++)
        for (y = 0; y < resolution_y; y++)
        {
            RGB &pix = img.pixel(x, y);

            Once(float(x), float(y), pix.r, pix.g, pix.b);

            if (int(current) % 100 == 0)
            {
                std::cout << current / total * 100.0 << "%\r";
                std::cout.flush();
            }
            current++;
        }

    /* save the image */
    img.save_to_ppm_file("output_test.ppm");

    // clean up
    for (int i = 0; i < gpPrimitives.size(); i++)
    {
        delete gpPrimitives[i];
    }

    return 0;
}

void Once(float xi, float yi, float &Rin, float &Gin, float &Bin)
{
    float alias_size = gRayTrace.GetAliasSize();
    int count = 0;
    float x, y;

    for (float xstep = xi; xstep < xi + 1; xstep += 1.0 / alias_size)
    {
        float xoffset = (1.0 / alias_size * (rand() / (RAND_MAX + 1.0)));
        x = xstep + (alias_size > 1.0 ? xoffset : 0.0);

        for (float ystep = yi; ystep < yi + 1; ystep += 1.0 / alias_size)
        {

            // Compute the offset due for aliasing
            float yoffset = (1.0 / alias_size * (rand() / (RAND_MAX + 1.0)));
            y = ystep + (alias_size > 1.0 ? yoffset : 0.0);
            count++; // for averaging the aliasing intensity

            // Compute the initial ray to the closest primitive
            smg::ray eye_ray;
            gRayTrace.DirectionVector(x, // Image Point X
                                      y, // Image Point Y
                                      eye_ray);

            smg::Vector color = gRayTrace.trace_ray(-1, eye_ray);

            Rin += color.x;
            Gin += color.y;
            Bin += color.z;
        }
    }

    Rin /= float(count);
    Gin /= float(count);
    Bin /= float(count);
}
