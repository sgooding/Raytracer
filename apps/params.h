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
    void read_input_file(std::vector<smg::primitive*>& gpPrimitives, 
                         const std::string& filename );


    int _resolution_x;
    int _resolution_y;

    inline const int& resolution_x() { return _resolution_x; }
    inline const int& resolution_y() { return _resolution_y; }

    float viewpoint[3];
    float screen_lower_left_corner[3];
    float screen_horizontal_vector[3];
    float screen_vertical_vector[3];
    float light_source[3];
    float light_intensity;
    float ambient_light_intensity;
    int number_of_primitives;
    int max_number_photons;
    int version;

};

#endif