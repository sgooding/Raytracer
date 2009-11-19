
/*
   Sean Gooding
   Project 1: Ray Tracing
   CSCI 441
 */


#include <math.h>
#include <fstream>
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
    double r,g,b;
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

static unsigned char clampnround ( double x )
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
    for ( int i=0; i<xsize*ysize; i++ )
    {
        unsigned char r = clampnround(256*rgb[i].r);
        unsigned char g = clampnround(256*rgb[i].g);
        unsigned char b = clampnround(256*rgb[i].b);
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
    ifstream ifs(filename.c_str());
    assert(ifs);

    double viewpoint[3];
    double screen_lower_left_corner[3];
    double screen_horizontal_vector[3];
    double screen_vertical_vector[3];
    double light_source[3];
    double light_intensity;
    double ambient_light_intensity;
    int number_of_primitives;
    int max_number_photons;


    int version;
    ifs >> version;
    std::cout << "Version: " << version << std::endl;
    std::cout << "Enable Photon Map: " << gRayTrace.GetEnablePhotonMapper() << std::endl;
    if( version == 1 )
    {
        ifs >> max_number_photons;
        if( max_number_photons > 0 )
        {
            gRayTrace.SetEnablePhotonMapper( true );
        } else
        {
            gRayTrace.SetEnablePhotonMapper( false );
        }
    }
    std::cout << "Enabled? " << gRayTrace.GetEnablePhotonMapper( ) << std::endl;
    if( version == 3 )
    {
        smg::Vector background_color;
        ifs >> background_color.x
            >> background_color.y
            >> background_color.z;
        gRayTrace.SetBackgroundColor( background_color );
    }
    ifs >> resolution_x >> resolution_y;
    ifs >> viewpoint[0] >> viewpoint[1] >> viewpoint[2];
    ifs >> screen_lower_left_corner[0] >> screen_lower_left_corner[1] >> screen_lower_left_corner[2];
    ifs >> screen_horizontal_vector[0] >> screen_horizontal_vector[1] >> screen_horizontal_vector[2];
    ifs >> screen_vertical_vector[0] >> screen_vertical_vector[1] >> screen_vertical_vector[2];
    ifs >> light_source[0] >> light_source[1] >> light_source[2];
    ifs >> light_intensity;
    ifs >> ambient_light_intensity;
    ifs >> number_of_primitives;

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
                    double center[3];
                    double radius;
                    double k_diffuse[3];
                    double k_ambient[3];
                    double k_specular;
                    double n_specular;
                    double index(0);
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
                    double a1[3];
                    double a2[3];
                    double a3[3];
                    double k_diffuse[3];
                    double k_ambient[3];
                    double k_specular;
                    double n_specular;
                    double index;
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
Once( double x, double y, double& R, double& G, double& B );

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
        gRayTrace.AliasSize( double(atoi(argv[2])) );
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
    double current(0.0);
    double total( resolution_x*resolution_y );
    for ( x=0; x<resolution_x; x++ )
        for ( y=0; y<resolution_y; y++ )
        {
            RGB &pix = img.pixel(x,y);

            Once(double(x), double(y), pix.r, pix.g, pix.b );

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
Once( double xi, double yi, double& Rin, double& Gin, double& Bin )
{
    double alias_size = gRayTrace.GetAliasSize();
    int count = 0;
    double x, y;

 
    for( double xstep = xi; xstep < xi+1; xstep += 1.0/alias_size )
    {
        double xoffset = (1.0/alias_size * (rand() / (RAND_MAX + 1.0)));
        x = xstep + (alias_size > 1.0?xoffset:0.0);

        for( double ystep = yi; ystep < yi+1; ystep += 1.0/alias_size )
        {

            // Compute the offset due for aliasing 
            double yoffset = (1.0/alias_size * (rand() / (RAND_MAX + 1.0)));
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

    Rin /= double( count );
    Gin /= double( count );
    Bin /= double( count );
}

