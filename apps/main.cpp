
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
#include "RenderEngine.h"
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
RenderEngine gRenderEngine;

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
    gRenderEngine.Render(img, gRayTrace, resolution_x, resolution_y);

    /* save the image */
    img.save_to_ppm_file("output_test.ppm");

    // clean up
    for (int i = 0; i < gpPrimitives.size(); i++)
    {
        delete gpPrimitives[i];
    }

    return 0;
}


