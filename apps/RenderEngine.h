#ifndef _RenderEngine_H
#define _RenderEngine_H

#include "Image.h"
#include "RayTrace.h"

class RenderEngine
{

public:
    void Render(image &img,
                smg::RayTrace &gRayTrace,
                int &resolution_x,
                int &resolution_y);

private:
    void Once(smg::RayTrace& gRayTrace,
            float xi, 
            float yi, 
            float &Rin, 
            float &Gin, 
            float &Bin);

};
#endif