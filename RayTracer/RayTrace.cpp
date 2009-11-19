//
// Author: Sean Gooding
// File: RayTrace.
// Date: Tue Jan 27 20:41:53 MST 2009
// Purpose:
//          A simple ray tracer
//
//
#define USE_PHOTON_MAP
//#undef USE_PHOTON_MAP

#include "RayTrace.h"
#include <assert.h>
#include <fstream>

namespace smg{

    RayTrace::RayTrace():
        mBackgroundColor(0.0,0.0,0.0),
        mPhotonMap(NULL),
        mCausticMap(NULL),
        mEnablePhotonMapper(true),
        mLightSource           (            ),
        mLightSourceIntensity  (            ),
        mViewPoint             (            ),
        mAmbientLight          (     0.0    ),
        mScreenLowerLeftCorner (            ),
        mScreenHorizontalVector(            ),
        mScreenVerticalVector  (            ),
        mResolutionY           (     0      ),
        mResolutionX           (     0      ),
        mAliasSize             (     1      ),
        mMaxNumberPhotons      (     0      ),
        mPrimitives            (     0      )
    {
    }

    void RayTrace::AddPrimitive( smg::primitive* pprimitive )
    {
        pprimitive->compute_probs();
        mPrimitives.push_back( pprimitive );
        (*(mPrimitives.end()-1))->Print();
    }

    RayTrace::~RayTrace()
    {

        for( int i = 0; i < mPrimitives.size(); i++ )
        {
            delete mPrimitives[i];
        }

        delete mPhotonMap;
        delete mCausticMap;
    }

    bool RayTrace::InitializeMap(int maxNumPhotons )
    {
        //mPhotonMap =  new PhotonMapHeinrik( maxNumPhotons );
        mPhotonMap =  new PhotonMapAnn( maxNumPhotons );
        mCausticMap =  new PhotonMapAnn( maxNumPhotons );
        return true;
    }


    bool RayTrace::TestInShadow( int PrimitiveInx, Vector location)
    {
        ray ray_between_location_light( location, (mLightSource - location).norm() );
        double distance;
        return FindNearestObject( ray_between_location_light, PrimitiveInx, distance );
    }

    ray RayTrace::GetRefractedRay(int prev_object, int curr_object,  const ray& original_ray, double distance)
    {
        bool entering(false);
        if( prev_object == -1 )
        {
            entering = true;
        } else if( (prev_object != curr_object) )
        {
            entering = true;
        }

        double current_medium(0.0);
        double next_medium(0.0);
        if( entering )
        {
            current_medium = 1.0; // air
            next_medium = mPrimitives[curr_object]->m.n_index;
            mPrimitives[curr_object]->m.inside = true;
        } else // exiting
        {
            current_medium = mPrimitives[prev_object]->m.n_index;
            next_medium = 1.0; // air
            mPrimitives[prev_object]->m.inside = false;
        }
        //std::cout << "Indecies = " << current_medium << "->"<<next_medium<<std::endl;

        double n1 = 1.0;//current_medium;
        double n2 = 1.5;//next_medium;
        double n = n1/n2;


        Vector N = mPrimitives[curr_object]->GetNormal(original_ray.get_endpoint(distance)); 
        if( !entering ) N = N*(-1.0);

        double c1 = -N.dot( original_ray.direction*(1.0) );


        double c1_sqr = c1*c1;
        double n_sqr = n*n;
        double radicand =  1.0-n_sqr*(1.0-c1_sqr);
        if( (radicand < 0.0) )
        {
            return GetReflectedRay( *mPrimitives[curr_object], 
                                    original_ray, 
                                    distance );

        //std::cout << "Radicand = " << radicand << std::endl;
        }


        double c2 = sqrt( radicand );

        Vector RefractedDirection( original_ray.direction*(n) + N*(n*c1-c2) );
        if( !entering ) RefractedDirection*(-1.0);
        //std::cout << "Normal: " << N << " Refracted: " << RefractedDirection << std::endl;
        ray refracted( original_ray.get_endpoint(distance), RefractedDirection.norm() );
        /*
        std::cout << "Inside: prev: curr" << (prev_object==-1?-1:mPrimitives[prev_object]->m.inside) << ":"
            << mPrimitives[curr_object]->m.inside << std::endl;
        std::cout << "Refracted " << prev_object << ", " << curr_object << std::endl;
        std::cout << "Ray: f:" << original_ray.origin << " d: "<< original_ray.direction <<std::endl
                  << ", t:  "<< refracted.origin      << " d: "<< refracted.direction <<std::endl
                  << " N dot( Refracted ) = " << N.dot(RefractedDirection) << std::endl
                  << " N dot( Incident ) = " << N.dot(original_ray.direction) << std::endl;
                  */

        return refracted;
    }

    ray RayTrace::GetDiffuseReflectedRay( const primitive& Primitive, const ray& original_ray, double distance )
    {

        double eta_1 = General::uniform_rand( 0.0, 1.0 );
        double eta_2 = General::uniform_rand( 0.0, 1.0 );
        Vector N = Primitive.GetNormal(original_ray.get_endpoint(distance)); 
        Vector ReflectedDirection( Vector::sphere2cart( acos( sqrt(eta_1) ), 2.0*M_PI*eta_2 ) );
        /*
        std::cout << "N: " << N
                  << "\nReflectedDirection: " << ReflectedDirection
                  << "\nPrimitive: " << Primitive.m.primitive_index << std::endl;

        abort();
        */

        //if( N.dot(ReflectedDirection )  < 0.0 ) ReflectedDirection = ReflectedDirection*(-1.0);
        return ray( original_ray.get_endpoint(distance), ReflectedDirection.norm() );
    }
    ray RayTrace::GetReflectedRay( const primitive& Primitive, const ray& original_ray, double distance )
    {

        Vector N = Primitive.GetNormal(original_ray.get_endpoint(distance)); 
        double c = -N.dot( original_ray.direction*(1.0) );
        Vector ReflectedDirection( original_ray.direction*(1.0) + ( N*(c*2.0) ) );
        ray reflected( original_ray.get_endpoint(distance), ReflectedDirection.norm() );
        return reflected;

    }

    Vector RayTrace::GetPointColor( const primitive& Primitive, const ray& origin, double distance )
    {
        // Compute the original color of the current object
        Vector color;
        Vector point_on_object( origin.get_endpoint(distance));
        Vector point_2_light( mLightSource -  point_on_object );
        double test_distance;
        int test_primitive( Primitive.m.primitive_index );

        bool in_shade( TestInShadow( test_primitive, point_on_object ) );

        Vector point_2_eye( origin.origin - point_on_object);

        ComputeBRDF( Primitive, point_on_object, point_2_eye.norm() , point_2_light.norm(), in_shade,  color.x, color.y, color.z );
        return color;
    }

    Vector RayTrace::trace_ray( const int current_object, ray original_ray )
    {
        static int count = 0;

        static int depth;
        if( current_object == -1 ){
            depth = 0;
            count++; }
        else depth++;
        if( depth >= 25 ) return mBackgroundColor;
        //if( depth > 2 ) 
            
        //std::cout << "depth = " << depth << std::endl;


        Vector point_color(0.0,0.0,0.0), reflect_color(0.0,0.0,0.0), refract_color(0.0,0.0,0.0);
        int new_object( current_object );
        double distance;
        bool object_found =  FindNearestObject( original_ray, new_object , distance );

        static bool stop=false;
        if( stop ) 
        {
            //std::cout << "object found = " << new_object << std::endl;
            //std::cout << "original_ray = " << original_ray.origin << ":" << original_ray.direction << std::endl;
            //std::cout << "original ray endpoint = " << original_ray.get_endpoint(distance);
            abort();
        }

        if( !object_found ) return mBackgroundColor;

        point_color = GetPointColor( *mPrimitives[new_object], original_ray, distance );

        if( mPrimitives[new_object]->m.mirror )
        {
            //std::cout << "Mirror Object: " << current_object << "->" << new_object << std::endl;
            reflect_color = trace_ray( new_object, GetReflectedRay( *mPrimitives[new_object], original_ray, distance ) );
            reflect_color = reflect_color*0.98;
        }
        if( mPrimitives[new_object]->m.glass)
        {
            int image_size( mResolutionX*mResolutionY );

            //std::cout << "Glass Object: " << current_object << "->" << new_object << std::endl;
            //stop = true;
            //std::cout << "ray direction: " << original_ray.direction << std::endl;
            //std::cout << "endpoint: " << original_ray.get_endpoint(distance) << std::endl;
            //std::cout << "image_size = " << 3.0*image_size << " count = " << count << std::endl;
            //if( ( (count >= image_size*0.0 ) &&  (count <= image_size*4.0) ) )
            static bool reflecting = true;
            if( reflecting )
            {
                //on = false;
                reflect_color = trace_ray( new_object, GetReflectedRay( *mPrimitives[new_object], original_ray, distance ) );
                reflect_color = reflect_color*0.30;
                reflecting = false;
            } else{

                refract_color = trace_ray( new_object, GetRefractedRay( current_object, new_object, original_ray, distance) );
                refract_color = refract_color*0.70;
                reflecting = true;
            }
            //std::cout << "Inside " << mPrimitives[new_object]->m.inside << std::endl;
        } 

        return point_color + reflect_color + refract_color;

    }

    void
        RayTrace::compute_multipath(const primitive& Primitive, ray origin, double distance,  int depth, Vector location, bool in_shadow, double&R, double&G, double&B)
        {

            Vector color = trace_ray( -1, origin);
            R = color.x;
            G = color.y;
            B = color.z;

        }

    void RayTrace::ComputeBRDF( const primitive& Primitive, const Vector& position, const Vector& incoming_direction, const Vector& outgoing_direction, bool in_shade, double& R, double& G, double& B )
    {
        // 1) Direct Illumination ( Ray Tracing ) No Bounce

        // 2) Specular and Glossy ( Ray Tracing 

        // 3) Caustic

        // 4) Multi-Diffuse (

        // Both incoming and outgoing are pointing away from the surface normal

        double Idr, Idg, Idb, Is;
        bool use_photon_map(mEnablePhotonMapper);

        //if( use_photon_map && !in_shade )
        if( use_photon_map  )
        {
            Vector N = Primitive.GetNormal(position ); // Normal to Surface
            float max_dist = .8;
            int   nphotons = mMaxNumberPhotons*0.1;

            Vector color(0.0,0.0,0.0);
#if 1
            mPhotonMap->compute_color( color,
                    Primitive,
                    incoming_direction,
                    position,
                    N,
                    max_dist,
                    nphotons );
            R = color.x;
            G = color.y;
            B = color.z;
#else
            float irrad[] = {0.0, 0.0, 0.0};
            float pos[] = {position.x, position.y, position.z};
            float norm[] = {N.x, N.y, N.z};

            mPhotonMap->irradiance_estimate(
                    irrad,
                    pos,
                    norm,
                    max_dist,
                    nphotons );
            R = irrad[0];
            G = irrad[1];
            B = irrad[2]; 
#endif

        }

        //bool use_ray_trace(use_photon_map);
        bool use_ray_trace(false);
        if( use_ray_trace ) 
        {
            if( !in_shade )
            {
                Vector L = outgoing_direction*(-1.0);
                Vector N = Primitive.GetNormal(position ).norm(); // Normal to Surface
                Vector V = incoming_direction;

                // Halfway Vector
                Vector H = (V+L).norm();

                // Specular
                Is = Primitive.m.k_spec*pow( H.dot(N), Primitive.m.n_spec);

                // Diffuse
                double normal_2_ls( N.dot( L ) );
                Idr =  Primitive.m.k_diff_R*( normal_2_ls );
                Idg =  Primitive.m.k_diff_G*( normal_2_ls );
                Idb =  Primitive.m.k_diff_B*( normal_2_ls );


                
                R += mLightSourceIntensity*(Idr + Is);// + Primitive.m.k_ambient_R*mAmbientLight; 
                G += mLightSourceIntensity*(Idg + Is);// + Primitive.m.k_ambient_G*mAmbientLight; 
                B += mLightSourceIntensity*(Idb + Is);// + Primitive.m.k_ambient_B*mAmbientLight;

            } 
        }

       // bool use_caustic_map( use_photon_map);
        bool use_caustic_map( mEnablePhotonMapper);
        if( 0 && use_caustic_map  )
        {
            Vector N = Primitive.GetNormal(position ); // Normal to Surface
            float max_dist = .08;
            int   nphotons = mMaxNumberPhotons*0.5;

            Vector color(0.0,0.0,0.0);
            mCausticMap->compute_color( color,
                        Primitive,
                        incoming_direction,
                        position,
                        N,
                        max_dist,
                        nphotons );
            R += color.x;
            G += color.y;
            B += color.z;
        }

        // Try just clamping
        //std::cout << "R = " << R << std::endl;
        if( R < 0.0 ) R = 0.0;
        if( G < 0.0 ) G = 0.0;
        if( B < 0.0 ) B = 0.0;
        R += Primitive.m.k_ambient_R*mAmbientLight; 
        G += Primitive.m.k_ambient_G*mAmbientLight; 
        B += Primitive.m.k_ambient_B*mAmbientLight;
        //if( R > 1.0 ) R = 1.0;
        //if( G > 1.0 ) G = 1.0;
        //if( B > 1.0 ) B = 1.0;
        //std::cout << "R: " << R << " G: " << G << " B: " << G << std::endl;

    }


    void
        RayTrace::TraceRay( const primitive& Primitive, 
                ray origin,
                double t, 
                Vector p, // location on the primitive
                bool in_shadow, // flag for in shadow
                double& R,
                double& G,
                double& B,
                bool test)
        {
            {
                int depth_search(0);
                double distance(0);
                compute_multipath( Primitive, 
                        ray( mViewPoint, (p-mViewPoint).norm() ), 
                        distance, 
                        depth_search, 
                        p, 
                        in_shadow, 
                        R, G, B );
                return;
            }

        }

    void 
        RayTrace::DirectionVector(double  x, double y, ray& eq )
        {
            //TODO: preprocess these viewpoints
            Vector dh = mScreenHorizontalVector*(double ( x)/double(mResolutionX));
            Vector dv = mScreenVerticalVector*(double(mResolutionY - y)/double(mResolutionY));
            eq.origin       = mViewPoint;
            eq.direction    = mScreenLowerLeftCorner + dh + dv - mViewPoint;
            eq.direction    = eq.direction*(1.0/eq.direction.mag());

        }


    void RayTrace::trace_multiple_paths( const int current_object, ray original_ray, const Vector& power, bool& added , std::list<int>& path_list)
    {
        static int depth = 0;
        if( current_object == -1 ) depth  = 0;
        depth++;
        //std::cout << "depth = " << depth << std::endl;


        int new_object( current_object );
        double distance(-1);

        //std::cout << "Tracing Multiple Paths" << std::endl;
        if( FindNearestObject( original_ray, new_object , distance ) )
        {
            // Choose what to do next
            double choice = General::uniform_rand(0.0, 1.0);
            material_prob::type reflection_choice;
            reflection_choice = mPrimitives[new_object]->prob_test( choice );

            float power_array[] = { power.x, power.y, power.z };
            Vector endpoint( original_ray.get_endpoint(distance) );
            float pos_array[] = { endpoint.x, endpoint.y, endpoint.z };
            float dir_array[] = {original_ray.direction.x,original_ray.direction.y,original_ray.direction.z  };
            Vector v_normal( mPrimitives[new_object]->GetNormal(endpoint) ); 
            

            switch( reflection_choice )
            {
                case material_prob::ABSORPTION:
                    {
                        if(0 && depth >= 3 )
                        {
                        std::cout << "Absorption" << std::endl;
                        std::cout << " new object = " << new_object << std::endl;
                        std::cout << "List Path Size = " << path_list.size() << std::endl;
                        for( std::list<int>::iterator ii = path_list.begin(); ii != path_list.end(); ii++ )
                            std::cout << "  " << *ii << std::endl;
                        abort();
                        }
                        if( path_list.size() >0 ) // only store direct

                        bool only_diffuse_reflection = ( std::find(path_list.begin(), path_list.end(),1 ) != path_list.end() );
                        bool only_specular_reflection = ( std::find(path_list.begin(), path_list.end(),2 ) != path_list.end() );
                        bool only_once = ( path_list.size() == 1 ); 
                        bool no_bounce = ( path_list.size() == 0 ); 
                        bool at_least_once = ( path_list.size() >= 1 );
                        bool only_caustic = ( std::find(path_list.begin(), path_list.end(),3 ) != path_list.end() );
                        bool store_all = true;
                                    
                        //if( only_diffuse_reflection && at_least_once ) // only store direct
                        if( !only_caustic && at_least_once  ) // only store direct
                        {
                            /*
                            if( mPhotonMap->get_stored_photons() % 100 == 0 ) 
                                std::cerr<< ".";
                            if( mPhotonMap->get_stored_photons() % 1000 == 0 ) 
                                std::cerr<< "*";
                                */


                            if( power_array[0] + power_array[1] + power_array[2] >= 0.0 )
                            {
                                                                                
                                added = true;
                              mPhotonMap->store( power_array, 
                                                   pos_array,
                                                   dir_array);
                            }

                            path_list.clear();

                        }
                        if( only_caustic ) // only store direct
                        {
                            if( power_array[0] + power_array[1] + power_array[2] >= 0.0 )
                            {

                                added = true;
                                mCausticMap->store( power_array, 
                                        pos_array,
                                        dir_array);
                            }

                            path_list.clear();

                        }
                        if(  (mCausticMap->get_stored_photons() + mPhotonMap->get_stored_photons()) % 100 == 0 )
                        {
                            double sum(mCausticMap->get_stored_photons() + mPhotonMap->get_stored_photons() );
                            double total( 2.0*mMaxNumberPhotons );
                            std::cout <<100.0*(sum/total)<< "%\t\t\r";
                            std::cout.flush();
                        }
                        break;
                    }
                case material_prob::DIFFUSE_REFLECTION:
                    {
                        //std::cout << "Diffuse" << std::endl;
                        path_list.push_back(1);
                        //ray new_direction( GetReflectedRay( *mPrimitives[new_object], original_ray, distance) );// endpoint, R );
                        ray new_direction( GetDiffuseReflectedRay( *mPrimitives[new_object], original_ray, distance) );// endpoint, R );
                        //Vector new_power(0.0,1.0,0.0);
                        Vector brdf( mPrimitives[new_object]->compute_brdf_d(endpoint, original_ray.direction, new_direction.direction ) );
                        Vector new_power(power.times(brdf));
                        //mPrimitives[new_object]->scale_power( reflection_choice, new_power);
                        //std::cout << "Power = " << power << " = > " << new_power << std::endl;
                        trace_multiple_paths( new_object, new_direction, new_power, added, path_list );
                        break;
                    }
                case material_prob::SPECULAR_REFLECTION:
                    {
                        //std::cout << "Specular" << std::endl;
                        path_list.push_back(2);
                        ray new_direction( GetReflectedRay( *mPrimitives[new_object], original_ray, distance) );// endpoint, R );
                        //Vector new_power(power);
                        //mPrimitives[new_object]->scale_power( reflection_choice, new_power);

                        Vector brdf( mPrimitives[new_object]->compute_brdf_s(endpoint, original_ray.direction, new_direction.direction ) );
                        Vector new_power(power.times(brdf));
                        trace_multiple_paths( new_object, new_direction, new_power, added, path_list );
                        break;
                    }
                case material_prob::TRANSMISSION:
                    {
                        //std::cout << "Transmission" << std::endl;
                        path_list.push_back(3);
                        static bool reflected(true);


                        if( reflected  )
                        {
                            ray new_direction( GetReflectedRay( *mPrimitives[new_object], original_ray, distance) );// endpoint, R );
                            Vector new_power(power*0.3);
                            trace_multiple_paths( new_object, new_direction, new_power, added, path_list );
                            reflected = false;

                        } else 
                        {
                            ray new_direction( GetRefractedRay(current_object, new_object, original_ray, distance) );
                            Vector new_power(power*0.7);
                            trace_multiple_paths( new_object, new_direction, new_power, added, path_list );
                            reflected = true;
                        }

                        break;
                    }

            }
        }

    }


    bool RayTrace::FindNearestObject( const ray& start, int& primitive_index, double& distance )
    {
        double t_min(9999.0);
        int nearest_primitive(-1);
        bool found(false);


        int current_index = -1;
        if( primitive_index >= 0 ) current_index = mPrimitives[primitive_index]->m.primitive_index; 
        bool is_glass(false);
        if( primitive_index >= 0 ) is_glass = mPrimitives[primitive_index]->m.glass;

        if( is_glass && mPrimitives[primitive_index]->m.inside )
        {
            ray inside_a_bit( start.origin + start.direction*0.00000001, start.direction );
            double t = mPrimitives[primitive_index]->intersection(inside_a_bit);
            //std::cout << "Passing: " << t << std::endl;
            distance = t;
            return true;
        }

        // Loop through all primitives 
        for( int i = 0; i < mPrimitives.size(); i++ ) {
            double t = mPrimitives[i]->intersection(start);

            if( (t > 0.0 && t < t_min) && 
                (i != current_index  )     ) 
            { 
                nearest_primitive = i;
                t_min = t;
            }
        }

        primitive_index = nearest_primitive;
        distance = t_min;

        return ( nearest_primitive >= 0  ); // is found
    }

    void RayTrace::EmitPhotons(void)
    {
        int n_photons = 0; // number of photons
        int n_caustic_photons = 0;
        double begin = -1.0;
        double end   = 1.0;


        bool enough_photons(false);
        double power_count(0.0);
        //std::ofstream outs( "light_source.txt", std::ofstream::app ); 
        bool top_half(true);
        std::cout << "Firing Photons" << std::endl;
        while( !enough_photons )
        {
            top_half = !top_half;
            smg::Vector photon_dir;
            photon_dir.z = 2.0*General::uniform_rand(0.0,1.0)-1.0;
            //photon_dir.z = 1.0*General::uniform_rand(0.0,1.0)*(top_half?1.0:-1);
            double t = 2.0*M_PI*General::uniform_rand(0.0,1.0);
            //double t = -1.0*M_PI*General::uniform_rand(0.0,1.0);
            double w = sqrt( 1.0 - photon_dir.z*photon_dir.z);
            photon_dir.x = w*cos(t);
            photon_dir.y = w*sin(t);
            // Scale every single fired photon
            //power_count += 1.0;
            //outs << photon_dir.x << ", " << photon_dir.y << ", " << photon_dir.z << std::endl;
            smg::ray photon( mLightSource, photon_dir ); 

            bool added(false);
            Vector photon_power( mLightSourceIntensity, mLightSourceIntensity, mLightSourceIntensity  );
            photon_power = photon_power * ( 1.0/mMaxNumberPhotons );
            std::list<int> path_list;
            trace_multiple_paths( -1, photon, photon_power, added, path_list  ); // Starting Ray
            //if( added ) power_count += 1.0;


            n_photons = mPhotonMap->get_stored_photons();
            n_caustic_photons = mCausticMap->get_stored_photons();
            //std::cout << "Store: " << n_photons << ":" << n_caustic_photons << std::endl;
            enough_photons = ( ( n_photons >= mMaxNumberPhotons ) && ( n_caustic_photons >= mMaxNumberPhotons ) );
        }

        //mPhotonMap->scale_photon_power( 1.0/power_count);

        std::cout << "Balancing.." << std::endl;
        mPhotonMap->balance();
    }

}
