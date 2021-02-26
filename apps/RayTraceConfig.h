#ifndef _RayTraceConfig_H
#define _RayTraceConfig_H

#include <string>
#include "RayTrace.h"

class RayTraceConfig
{
public:
    void read_input_file( std::string filename,
                          smg::RayTrace& aRayTrace,
                          std::vector<smg::primitive *>& aPrimitives,
                          int& resolution_x,
                          int& resolution_y );
};

#endif