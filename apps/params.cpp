#include "params.h"

using namespace smg;

void Params::read_input_file( std::vector<primitive*>& gpPrimitives,
                             const std::string& filename)
{
    ifstream ifs_file(filename.c_str());
    assert(ifs_file);

    //int version;
    std::string line;
    std::stringstream ifs;
    while (ifs_file)
    {
        std::getline(ifs_file, line);
        if (not line.empty() and line.find('#') == std::string::npos)
        {
            ifs << line << std::endl;
        }
    }

    ifs >> version;

    std::cout << "Version: " << version << std::endl;
    if (version == 1)
    {
        ifs >> max_number_photons;
        std::cout << "Max Number Photons: " << max_number_photons << std::endl;
        //if (max_number_photons > 0)
        //{
        //    gRayTrace.SetEnablePhotonMapper(true);
        //}
        //else
        //{
        //    gRayTrace.SetEnablePhotonMapper(false);
        //}
    }
    //std::cout << "Photon Mapper Enabled: " << gRayTrace.GetEnablePhotonMapper() << std::endl;

    ifs >> _resolution_x >> _resolution_y;
    std::cout << "Resolution: " << _resolution_x << ", " << _resolution_y << std::endl;
    ifs >> viewpoint[0] >> viewpoint[1] >> viewpoint[2];
    std::cout << "Viewpoint: " << viewpoint[0] << ", " << viewpoint[1] << ", " << viewpoint[2] << std::endl;
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
    //gRayTrace.SetEnableRayTrace(enable_ray_trace);

    int enable_photon_map(0);
    ifs >> enable_photon_map;
    //gRayTrace.SetEnablePhotonMap(enable_photon_map);

    int enable_caustic(0);
    ifs >> enable_caustic;
    //gRayTrace.SetEnableCaustic(enable_caustic);

    //gRayTrace.ResolutionX(resolution_x);
    //gRayTrace.ResolutionY(resolution_y);

    //gRayTrace.ViewPoint(smg::Vector(viewpoint[0], viewpoint[1], viewpoint[2]));
    //gRayTrace.LightSource(smg::Vector(light_source[0], light_source[1], light_source[2]));
    //gRayTrace.LightSourceIntensity(light_intensity);
    //gRayTrace.AmbientLight(ambient_light_intensity);
    //gRayTrace.ScreenLowerLeftCorner(smg::Vector(
    //    screen_lower_left_corner[0],
    //    screen_lower_left_corner[1],
    //    screen_lower_left_corner[2]));
    //gRayTrace.ScreenHorizontalVector(smg::Vector(
    //    screen_horizontal_vector[0],
    //    screen_horizontal_vector[1],
    //    screen_horizontal_vector[2]));
    //gRayTrace.ScreenVerticalVector(smg::Vector(
    //    screen_vertical_vector[0],
    //    screen_vertical_vector[1],
    //    screen_vertical_vector[2]));

    //gRayTrace.MaxNumberPhotons(max_number_photons);

    // Initialize the photon map
    if (gRayTrace.GetEnablePhotonMapper())
    {
        gRayTrace.InitializeMap(gRayTrace.GetMaxNumberPhotons());
    }

    // save all this info to your datastructures or global variables here
    std::cout << "Number of Primitives: " << number_of_primitives << std::endl;

    for (int i = 0; i < number_of_primitives; i++)
    {
        char primitive_type;
        ifs >> primitive_type;
        switch (primitive_type)
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

            if (version == 1)
                ifs >> mirror_glass;
            ifs >> center[0] >> center[1] >> center[2];
            ifs >> radius;
            ifs >> k_diffuse[0] >> k_diffuse[1] >> k_diffuse[2];
            ifs >> k_ambient[0] >> k_ambient[1] >> k_ambient[2];
            ifs >> k_specular >> n_specular;
            //ifs >> index >> mirror;

            // add the sphere to your datastructures (primitive list, sphere list or such) here
            smg::primitive *pprimitive = new smg::sphere(smg::Vector(center[0], center[1], center[2]),
                                                         radius);

            pprimitive->m.k_diff_R = k_diffuse[0];
            pprimitive->m.k_diff_G = k_diffuse[1];
            pprimitive->m.k_diff_B = k_diffuse[2];
            pprimitive->m.k_ambient_R = k_ambient[0];
            pprimitive->m.k_ambient_G = k_ambient[1];
            pprimitive->m.k_ambient_B = k_ambient[2];
            pprimitive->m.k_spec = k_specular;
            pprimitive->m.n_spec = n_specular;

            if (1)
            {

                pprimitive->m.n_index = index;
                pprimitive->m.primitive_index = i;
                pprimitive->m.mirror = (mirror_glass == 1);
                pprimitive->m.glass = (mirror_glass == 2);
                if (pprimitive->m.glass)
                {
                    std::cout << "Glass" << std::endl;
                    pprimitive->m.n_index = 1.5;
                }
            }

            gpPrimitives.push_back(pprimitive);
            (*(gpPrimitives.end() - 1))->Print();

            smg::sphere *copy = dynamic_cast<smg::sphere *>(pprimitive);
            gRayTrace.AddPrimitive(new smg::sphere(*copy));
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

            if (version == 1)
                ifs >> mirror_glass;
            ifs >> a1[0] >> a1[1] >> a1[2];
            ifs >> a2[0] >> a2[1] >> a2[2];
            ifs >> a3[0] >> a3[1] >> a3[2];
            ifs >> k_diffuse[0] >> k_diffuse[1] >> k_diffuse[2];
            ifs >> k_ambient[0] >> k_ambient[1] >> k_ambient[2];
            ifs >> k_specular >> n_specular;
            //ifs >> index >> mirror;

            // add the triangle to your datastructure (primitive list, sphere list or such) here

            smg::primitive *pprimitive = new smg::triangle(smg::Vector(a1[0], a1[1], a1[2]),
                                                           smg::Vector(a2[0], a2[1], a2[2]),
                                                           smg::Vector(a3[0], a3[1], a3[2]),
                                                           gRayTrace.GetViewPoint());

            pprimitive->m.k_diff_R = k_diffuse[0];
            pprimitive->m.k_diff_G = k_diffuse[1];
            pprimitive->m.k_diff_B = k_diffuse[2];
            pprimitive->m.k_ambient_R = k_ambient[0];
            pprimitive->m.k_ambient_G = k_ambient[1];
            pprimitive->m.k_ambient_B = k_ambient[2];

            pprimitive->m.k_spec = k_specular;
            pprimitive->m.n_spec = n_specular;

            if (1)
            {
                pprimitive->m.primitive_index = i;
                pprimitive->m.n_index = index;
                pprimitive->m.mirror = (mirror_glass == 1);
                pprimitive->m.glass = (mirror_glass == 2);

                if (pprimitive->m.glass)
                    pprimitive->m.n_index = 1.5;
            }

            gpPrimitives.push_back(pprimitive);
            (*(gpPrimitives.end() - 1))->Print();

            smg::triangle *copy = dynamic_cast<smg::triangle *>(pprimitive);
            gRayTrace.AddPrimitive(new smg::triangle(*copy));
        }
        break;
        default:
            std::cout << "Primitive Failed " << primitive_type << std::endl;
            assert(0);
        }
    }
}