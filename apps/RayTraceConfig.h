#ifndef _RayTraceConfig_H
#define _RayTraceConfig_H

#include <string>
#include <sstream>

#include "RayTrace.h"

class RayTraceConfig
{
public:
    void read_input_file( std::string filename,
                          smg::RayTrace& aRayTrace);

    void load_brdf(std::stringstream& ifs, smg::primitive* pprimitive);
};

#endif