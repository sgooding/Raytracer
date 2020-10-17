
/*
   Sean Gooding
   Project 1: Ray Tracing
   CSCI 441
 */

#include <iostream>
#include <vector>

#include "Primitives.h"
#include "RayTrace.h"
#include "image.h"
#include "params.h"

#include "CImg.h"

using namespace std;



/* ----------- Reading the input file ---------- */

// global variables

/* ----------- main function ---------- */
int main(int argc, char *argv[])
{

    std::string filename("input.txt");
    float alias_size(2.0);
    if (argc == 2)
    {
        filename = argv[1];
    }
    else if(argc == 3)
    {
        filename = argv[1];
        alias_size = float(atoi(argv[2]));
    }
    else if(argc > 3)
    {
        std::cerr << "Wrong number of args." << std::endl;
        return 0;
    }

    smg::RayTrace raytrace;

    raytrace.AliasSize(2.0);

    std::vector<smg::primitive*> primitives;

    Params params;
    params.read_input_file(raytrace, 
                           primitives, 
                           filename);

    if (raytrace.GetEnablePhotonMapper())
    {
        raytrace.EmitPhotons();
    }

    std::cout << "Begin Rendering" << std::endl;

    image img(params.resolution_x(), params.resolution_y());

    raytrace.RenderImage(img);

    /* save the image */
    img.save_to_ppm_file("output_test.ppm");

    // clean up
    for (int i = 0; i < primitives.size(); i++)
    {
        delete primitives[i];
    }

    // display code
    using namespace cimg_library;
    cimg_library::CImg<unsigned char> original_image("output_test.ppm");
    CImgDisplay local(original_image, "Output");
    while (!local.is_closed())
    {
        local.wait();
    }
    original_image.save("output_test.bmp");
    return 0;
}