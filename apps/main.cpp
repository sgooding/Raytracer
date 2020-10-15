
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
#include <vector>
#include <stdlib.h>

using namespace std;

/* ------------ typedefs -------------- */

typedef struct {
    float r,g,b;
}
RGB;

class image {
    int xsize,ysize; // resolution
    RGB *rgb;        // pixel intensities
    public:
    image ( int m, int n );       // allocates image of specified size
    RGB &pixel ( int i, int j );  // access to a specific pixel
    //void save_to_ppm_file ( char *filename );
    void save_to_ppm_file ( std::string filename );
};


/* ----------- image class: methods ---------- */

image::image ( int m, int n ) : xsize(m), ysize(n), rgb(NULL)
{
    rgb = new RGB[m*n];
}

/* ----------------------- */

RGB &image::pixel ( int i, int j )
{
    return rgb[i+xsize*j];
}

/* ----------------------- */

static unsigned char clampnround ( float x )
{
    if (x>255)
        x = 255;
    if (x<0) 
        x = 0;
    return (unsigned char)floor(x+.5);
}



/* ----------------------- */

void image::save_to_ppm_file ( std::string filename )
{
    ofstream ofs(filename.c_str(),ios::binary);
    assert(ofs);
    ofs << "P6" << endl;
    ofs << xsize << " " << ysize << endl << 255 << endl;


    unsigned char min_rgb[3] = {255,255,255};
    unsigned char max_rgb[3] = {0,0,0};

    for ( int i=0; i<xsize*ysize; i++ )
    {
        unsigned char r = clampnround(256*rgb[i].r);
        unsigned char g = clampnround(256*rgb[i].g);
        unsigned char b = clampnround(256*rgb[i].b);

        min_rgb[0] = std::min(min_rgb[0],r);
        min_rgb[1] = std::min(min_rgb[1],g);
        min_rgb[2] = std::min(min_rgb[2],b);

        max_rgb[0] = std::max(max_rgb[0],r);
        max_rgb[1] = std::max(max_rgb[1],g);
        max_rgb[2] = std::max(max_rgb[2],b);
    }

    float r_scale = 1.0;
    float g_scale = 1.0;
    float b_scale = 1.0;
    for ( int i=0; i<xsize*ysize; i++ )
    {
        unsigned char r = clampnround(256*rgb[i].r*r_scale);
        unsigned char g = clampnround(256*rgb[i].g*g_scale);
        unsigned char b = clampnround(256*rgb[i].b*b_scale);
        ofs.write((char*)&r,sizeof(char));
        ofs.write((char*)&g,sizeof(char));
        ofs.write((char*)&b,sizeof(char));
    }
}


/* ----------- Reading the input file ---------- */

// global variables
int resolution_x, resolution_y;
std::vector<smg::primitive*> gpPrimitives;
smg::RayTrace gRayTrace;

// ... and the input file reading function
void read_input_file(std::string filename)
{
    ifstream ifs_file(filename.c_str());
    assert(ifs_file);

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
    std::string line;
    std::stringstream ifs;
    while(ifs_file)
    {
        std::getline(ifs_file,line);
        if(not line.empty() and line.find('#') == std::string::npos)
        {
            ifs << line << std::endl;
        }
    }
    ifs >> version;
    std::cout << "Version: " << version << std::endl;
    if( version == 1 )
    {
        ifs >> max_number_photons;
        std::cout << "Max Number Photons: " << max_number_photons << std::endl;
        if( max_number_photons > 0 )
        {
            gRayTrace.SetEnablePhotonMapper( true );
        } else
        {
            gRayTrace.SetEnablePhotonMapper( false );
        }
    }
    std::cout << "Photon Mapper Enabled: " << gRayTrace.GetEnablePhotonMapper( ) << std::endl;

    ifs >> resolution_x >> resolution_y;
    std::cout << "Resolution: " << resolution_x << ", " << resolution_y << std::endl;
    ifs >> viewpoint[0] >> viewpoint[1] >> viewpoint[2];
    std::cout << "Viewpoint: " << viewpoint[0]<<", " << viewpoint[1] <<", " << viewpoint[2] << std::endl;
    ifs >> screen_lower_left_corner[0] >> screen_lower_left_corner[1] >> screen_lower_left_corner[2];
    ifs >> screen_horizontal_vector[0] >> screen_horizontal_vector[1] >> screen_horizontal_vector[2];
    ifs >> screen_vertical_vector[0] >> screen_vertical_vector[1] >> screen_vertical_vector[2];
    ifs >> light_source[0] >> light_source[1] >> light_source[2];
    std::cout << "Light Source: " << light_source[0] << ", "
                                  << light_source[1] << ", "
                                  << light_source[2] << ", "
                                  << std::endl;
    ifs >> light_intensity;
    std::cout << "Light Intensity : " << light_intensity << std::endl;
    ifs >> ambient_light_intensity;
    ifs >> number_of_primitives;

    int enable_ray_trace(0);
    ifs >> enable_ray_trace;
    gRayTrace.SetEnableRayTrace(enable_ray_trace);

    int enable_photon_map(0);
    ifs >> enable_photon_map;
    gRayTrace.SetEnablePhotonMap(enable_photon_map);

    int enable_caustic(0);
    ifs >> enable_caustic;
    gRayTrace.SetEnableCaustic(enable_caustic);

    gRayTrace.ResolutionX( resolution_x );
    gRayTrace.ResolutionY( resolution_y );
    gRayTrace.ViewPoint( smg::Vector( viewpoint[0] , viewpoint[1] , viewpoint[2] ) );
    gRayTrace.LightSource( smg::Vector( light_source[0] , light_source[1] , light_source[2] ) );
    gRayTrace.LightSourceIntensity( light_intensity );
    gRayTrace.AmbientLight( ambient_light_intensity );
    gRayTrace.ScreenLowerLeftCorner(smg::Vector(
                screen_lower_left_corner[0],
                screen_lower_left_corner[1],
                screen_lower_left_corner[2]
                ) );
    gRayTrace.ScreenHorizontalVector(smg::Vector(
                screen_horizontal_vector[0],
                screen_horizontal_vector[1],
                screen_horizontal_vector[2]
                ) );
    gRayTrace.ScreenVerticalVector(smg::Vector(
                screen_vertical_vector[0],
                screen_vertical_vector[1],
                screen_vertical_vector[2]
                ) );

    gRayTrace.MaxNumberPhotons( max_number_photons );

    // Initialize the photon map
    if( gRayTrace.GetEnablePhotonMapper() )
    {
        gRayTrace.InitializeMap( gRayTrace.GetMaxNumberPhotons() );
    }

    // save all this info to your datastructures or global variables here
    std::cout << "Number of Primitives: " << number_of_primitives << std::endl;

    for ( int i=0; i<number_of_primitives; i++ )
    {
        char primitive_type;
        ifs >> primitive_type;
        switch(primitive_type)
        {
            case 's':
            case 'S':
                {
                    float center[3];
                    float radius;
                    float k_diffuse[3];
                    float k_ambient[3];
                    float k_specular;
                    float n_specular;
                    float index(0);
                    int mirror_glass(0);



                    if( version == 1 )
                        ifs >> mirror_glass;
                    ifs >> center[0] >> center[1] >> center[2];
                    ifs >> radius;
                    ifs >> k_diffuse[0] >> k_diffuse[1] >> k_diffuse[2];
                    ifs >> k_ambient[0] >> k_ambient[1] >> k_ambient[2];
                    ifs >> k_specular >> n_specular;
                    //ifs >> index >> mirror;

                    // add the sphere to your datastructures (primitive list, sphere list or such) here
                    smg::primitive* pprimitive = new smg::sphere( smg::Vector( center[0], center[1], center[2] ),
                            radius );

                    pprimitive->m.k_diff_R = k_diffuse[0];
                    pprimitive->m.k_diff_G = k_diffuse[1];
                    pprimitive->m.k_diff_B = k_diffuse[2];
                    pprimitive->m.k_ambient_R = k_ambient[0];
                    pprimitive->m.k_ambient_G = k_ambient[1];
                    pprimitive->m.k_ambient_B = k_ambient[2];
                    pprimitive->m.k_spec = k_specular;
                    pprimitive->m.n_spec = n_specular;

                    if( 1 ) {
                        
                    pprimitive->m.n_index = index;
                    pprimitive->m.primitive_index = i;
                    pprimitive->m.mirror = (mirror_glass == 1);
                    pprimitive->m.glass = (mirror_glass == 2);
                    if( pprimitive->m.glass )
                    {
                        std::cout << "Glass" << std::endl;
                        pprimitive->m.n_index = 1.5;
                    }
                    
                    }

                    gpPrimitives.push_back(pprimitive);
                    (*(gpPrimitives.end()-1))->Print();

                    smg::sphere* copy = dynamic_cast< smg::sphere* >( pprimitive );
                    gRayTrace.AddPrimitive( new smg::sphere(*copy) );
                }
                break;
            case 'T':
            case 't':
                {
                    float a1[3];
                    float a2[3];
                    float a3[3];
                    float k_diffuse[3];
                    float k_ambient[3];
                    float k_specular;
                    float n_specular;
                    float index;
                    int mirror_glass(0);



                    if( version == 1 )
                        ifs >> mirror_glass;
                    ifs >> a1[0] >> a1[1] >> a1[2];
                    ifs >> a2[0] >> a2[1] >> a2[2];
                    ifs >> a3[0] >> a3[1] >> a3[2];
                    ifs >> k_diffuse[0] >> k_diffuse[1] >> k_diffuse[2];
                    ifs >> k_ambient[0] >> k_ambient[1] >> k_ambient[2];
                    ifs >> k_specular >> n_specular; 	    
                    //ifs >> index >> mirror;

                    // add the triangle to your datastructure (primitive list, sphere list or such) here

                    smg::primitive* pprimitive = new smg::triangle( smg::Vector(a1[0], a1[1], a1[2] ),
                            smg::Vector(a2[0], a2[1], a2[2] ),
                            smg::Vector(a3[0], a3[1], a3[2] ),
                            gRayTrace.GetViewPoint() );

                    pprimitive->m.k_diff_R = k_diffuse[0];
                    pprimitive->m.k_diff_G = k_diffuse[1];
                    pprimitive->m.k_diff_B = k_diffuse[2];
                    pprimitive->m.k_ambient_R = k_ambient[0];
                    pprimitive->m.k_ambient_G = k_ambient[1];
                    pprimitive->m.k_ambient_B = k_ambient[2];

                    pprimitive->m.k_spec = k_specular;
                    pprimitive->m.n_spec = n_specular;

                    if(1){
                    pprimitive->m.primitive_index = i;
                    pprimitive->m.n_index = index;
                    pprimitive->m.mirror = (mirror_glass == 1);
                    pprimitive->m.glass = (mirror_glass == 2);

                    if( pprimitive->m.glass )
                        pprimitive->m.n_index = 1.5;

                    }

                    gpPrimitives.push_back(pprimitive);
                    (*(gpPrimitives.end()-1))->Print();

                    smg::triangle* copy = dynamic_cast< smg::triangle* >( pprimitive );
                    gRayTrace.AddPrimitive( new smg::triangle(*copy) );
                }
                break;
            default:
                std::cout << "Primitive Failed " << primitive_type << std::endl;
                assert(0);
        }
    }
}

/* ----------- function prototypes ---------- */
void
Once( float x, float y, float& R, float& G, float& B );

void emit_photons(void);

/* ----------- main function ---------- */
int main ( int argc, char *argv[] )
{
    std::string filename("input.txt");
    if( argc == 2 )
    {
        filename = argv[1];
        gRayTrace.AliasSize( 2.0 );
    }
    else if( argc == 3 )
    {
        filename = argv[1];
        gRayTrace.AliasSize( float(atoi(argv[2])) );
    }
    else
    {
        std::cout << "Usage: " << argv[0] << " input.txt aliasing(default = 2)" << std::endl;
        return 0;
    }

    int x,y;

    read_input_file(filename);

    if( gRayTrace.GetEnablePhotonMapper() )
    {
        gRayTrace.EmitPhotons();
    }

    std::cout << "Begin Rendering" << std::endl;

    image img(resolution_x,resolution_y);
    float current(0.0);
    float total( resolution_x*resolution_y );
    for ( x=0; x<resolution_x; x++ )
        for ( y=0; y<resolution_y; y++ )
        {
            RGB &pix = img.pixel(x,y);

            Once(float(x), float(y), pix.r, pix.g, pix.b );

            if( int(current) % 100 == 0 ) {
                std::cout << current/total*100.0 << "%\r";
                std::cout.flush();
            }
            current++;

        }

    /* save the image */
    img.save_to_ppm_file("output_test.ppm");


    // clean up
    for( int i = 0; i < gpPrimitives.size(); i++ )
    {
        delete gpPrimitives[i];
    }

    return 0;
}



void
Once( float xi, float yi, float& Rin, float& Gin, float& Bin )
{
    float alias_size = gRayTrace.GetAliasSize();
    int count = 0;
    float x, y;
 
    for( float xstep = xi; xstep < xi+1; xstep += 1.0/alias_size )
    {
        float xoffset = (1.0/alias_size * (rand() / (RAND_MAX + 1.0)));
        x = xstep + (alias_size > 1.0?xoffset:0.0);

        for( float ystep = yi; ystep < yi+1; ystep += 1.0/alias_size )
        {

            // Compute the offset due for aliasing 
            float yoffset = (1.0/alias_size * (rand() / (RAND_MAX + 1.0)));
            y = ystep + (alias_size > 1.0?yoffset:0.0);
            count++; // for averaging the aliasing intensity


            // Compute the initial ray to the closest primitive
            smg::ray eye_ray;
            gRayTrace.DirectionVector( x, // Image Point X 
                                       y, // Image Point Y
                                       eye_ray ); 

            smg::Vector color = gRayTrace.trace_ray( -1, eye_ray);

            Rin += color.x;
            Gin += color.y;
            Bin += color.z;

        }
    }

    Rin /= float( count );
    Gin /= float( count );
    Bin /= float( count );
}

