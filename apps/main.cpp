
/*
   Sean Gooding
   Project 1: Ray Tracing
   CSCI 441
 */

#include <iostream>
#include "Primitives.h"
#include "RayTrace.h"
#include "RayTraceConfig.h"
#include "RenderEngine.h"
#include "Image.h"
#include <vector>

using namespace std;

/* ----------- main function ---------- */
int main(int argc, char *argv[])
{

    // global variables
    smg::RayTrace gRayTrace;
    RayTraceConfig gRayTraceConfig;
    RenderEngine gRenderEngine;

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

    gRayTraceConfig.read_input_file(filename, gRayTrace);

    if (gRayTrace.GetEnablePhotonMapper())
    {
        gRayTrace.EmitPhotons();
    }

    image img(gRayTrace.GetResolution().x, gRayTrace.GetResolution().y);

    std::cout << "Begin Rendering" << std::endl;
    gRenderEngine.Render(img, gRayTrace);

    /* save the image */
    img.save_to_ppm_file("output_test.ppm");


    return 0;
}


