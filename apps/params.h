#ifndef __Params__H
#define __Params__H

#include "Primitives.h"
#include "RayTrace.h"

#include <fstream>
#include <iostream>
#include <assert.h>
#include <vector>
#include <sstream>

using namespace std;
using namespace smg;

class Params
{
public:
    // ... and the input file reading function
    void read_input_file(smg::RayTrace& gRayTrace, 
                         std::vector<smg::primitive*>& gpPrimitives, 
                         const std::string& filename );


    int _resolution_x;
    int _resolution_y;

    inline const int& resolution_x() { return _resolution_x; }
    inline const int& resolution_y() { return _resolution_y; }
};

#endif