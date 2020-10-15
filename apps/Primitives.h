//
// Author: Sean Gooding
// File: Primitives.h
// Date: Tue Jan 27 20:41:53 MST 2009
// Purpose:
//          A few ray tracing primitives
//
//
#ifndef smg_Primitives_H_
#define smg_Primitives_H_

#include "Vector.h"

namespace smg {

int get_unique_name();
    
class material_prob{
    public:

        enum type { DIFFUSE_REFLECTION, 
                    SPECULAR_REFLECTION, 
                    ABSORPTION, 
                    TRANSMISSION, 
                    UNKNOWN };
                    
        material_prob():
            absorption(1.0),
            diffuse(0.0),
            specular(0.0),
            transmission(0.0),
            total(0.0)
        {
        }
        float absorption;
        float diffuse;
        float specular;
        float transmission;
        float total;
};

class material {
    public:
        material_prob prob;

        int   primitive_index;
        float k_diff_R;
        float k_diff_G;
        float k_diff_B;
        float k_ambient_R;
        float k_ambient_G;
        float k_ambient_B;
        float k_spec;
        float n_spec;
        float n_index; // index of refraction
        bool   mirror;
        bool   glass;
        bool   refracted;
        bool   inside;

        material() :
                    prob(),
                    primitive_index(0.0), 
                    k_diff_R       (0.0),  
                    k_diff_G       (0.0),  
                    k_diff_B       (0.0),  
                    k_ambient_R    (0.0),  
                    k_ambient_G    (0.0),  
                    k_ambient_B    (0.0),  
                    k_spec         (0.0),  
                    n_spec         (0.0),  
                    n_index        (1.5),  
                    mirror         (false),  
                    glass          (false),  
                    refracted      (false),  
                    inside         (false)  
    {
    }
};


    class ray {
        public:
            ray():
            origin(0.0,0.0,0.0),
            direction(0.0,0.0,0.0){}

            ray( Vector o, Vector d ) :
                origin(o),
                direction(d) {}

            Vector get_endpoint(const float t) const;

            Vector origin;
            Vector direction;
    };

    class primitive {
        public:
            primitive();
            virtual float intersection( ray r ) const = 0 ;
            virtual void Print(std::ostream& strm) const;
            virtual void Print() const;
            material m;
            virtual Vector GetNormal(const Vector& point ) const = 0;
            virtual ~primitive();
            void compute_probs();
            material_prob::type prob_test( float element );
            void scale_power( material_prob::type transmission_type, Vector& power );
            float compute_brdf( const Vector& incoming, const Vector& outgoing );
            Vector compute_brdf_r( const Vector& position, const Vector& incident, const Vector& reflected ) const;
            Vector compute_brdf_s( const Vector& position, const Vector& incident, const Vector& reflected ) const;
            Vector compute_brdf_d( const Vector& position, const Vector& incident, const Vector& reflected ) const;

            std::string name;
    };

    class plane : public primitive{
        public:

            plane();
            plane(Vector norm, float d);
            ~plane();

            void Print(std::ostream& strm) const;
            void Print();

            float intersection( ray r ) const;

            Vector GetNormal(const Vector& point ) const;
            Vector norm;
            Vector nnorm;
            float d;
    };

    class triangle : public primitive {
        public:
            triangle();
            triangle(Vector a, Vector b, Vector c, Vector view);

            ~triangle();
            
            void Print(std::ostream& strm) const;
            void Print();

            float intersection( ray r ) const;
            Vector a1;
            Vector a2;
            Vector a3;
            Vector mNorm;
            float mD;
            
            Vector GetNormal(const Vector& point ) const;
    };

    class sphere : public primitive{
        public:
            inline void printon(bool print){mprint = print;}
            sphere();
            sphere(Vector c, float r);
            ~sphere();

            void Print(std::ostream& strm) const;
            void Print();


            float intersection( ray r ) const;
            Vector center;
            float radius;
            
            Vector GetNormal(const Vector& point ) const;
        private:
            bool mprint;
    };

    class SchlickBRDF {
        public:
            void ComputeBRDF( const Vector& Normal,
                         const Vector& Win,
                         const Vector& Wout );

    };

    class General {
        public:
            static float uniform_rand(float begin, float end); 
    };
}

#endif
