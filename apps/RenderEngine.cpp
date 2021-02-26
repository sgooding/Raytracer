#include "RenderEngine.h"
#include "Types.h"
#include <iostream>

using namespace smg;

void RenderEngine::Render(image& img, RayTrace& gRayTrace)
{
    int resolution_x(gRayTrace.GetResolution().x);
    int resolution_y(gRayTrace.GetResolution().y);
    float current(0.0);
    float total(resolution_x * resolution_y);
    for (int x = 0; x < resolution_x; x++)
    {
        for (int y = 0; y < resolution_y; y++)
        {
            RGB &pix = img.pixel(x, y);

            Once(gRayTrace, float(x), float(y), pix.r, pix.g, pix.b);

            if (int(current) % 100 == 0)
            {
                std::cout << current / total * 100.0 << "%\r";
                std::cout.flush();
            }
            current++;
        }
    }   
}

void RenderEngine::Once( 
           RayTrace& gRayTrace,
           float xi, 
           float yi, 
           float &Rin, 
           float &Gin, 
           float &Bin )
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