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
    class material_prob{
        public:
            enum type { DIFFUSE_REFLECTION, 
                        SPECULAR_REFLECTION, 
                        ABSORPTION, 
                        TRANSMISSION, 
                        UNKNOWN };
            double absorption;
            double diffuse;
            double specular;
            double transmission;
            double total;
    };

    class material {
        public:
            material_prob prob;

            int    primitive_index              ;
            double k_diff_R              ;
            double k_diff_G              ;
            double k_diff_B              ;
            double k_ambient_R              ;
            double k_ambient_G              ;
            double k_ambient_B              ;
            double k_spec              ;
            double n_spec              ;
            double n_index              ; // index of refraction
            bool   mirror              ;
            bool   glass              ;
            bool   refracted              ;
            bool   inside              ;

            material() :
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

            Vector get_endpoint(const double t) const;

            Vector origin;
            Vector direction;
    };

    class primitive {
        public:
            primitive();
            virtual double intersection( ray r ) const = 0 ;
            virtual void Print(std::ostream& strm) const;
            virtual void Print() const;
            material m;
            virtual Vector GetNormal(const Vector& point ) const = 0;
            virtual ~primitive();
            void compute_probs();
            material_prob::type prob_test( double element );
            void scale_power( material_prob::type transmission_type, Vector& power );
            double compute_brdf( const Vector& incoming, const Vector& outgoing );
            Vector compute_brdf_r( const Vector& position, const Vector& incident, const Vector& reflected ) const;
            Vector compute_brdf_s( const Vector& position, const Vector& incident, const Vector& reflected ) const;
            Vector compute_brdf_d( const Vector& position, const Vector& incident, const Vector& reflected ) const;
    };

    class plane : public primitive{
        public:

            plane();
            plane(Vector norm, double d);
            ~plane();

            void Print(std::ostream& strm) const;
            void Print();

            double intersection( ray r ) const;

            Vector GetNormal(const Vector& point ) const;
            Vector norm;
            Vector nnorm;
            double d;
    };

    class triangle : public primitive {
        public:
            triangle();
            triangle(Vector a, Vector b, Vector c, Vector view);

            ~triangle();
            
            void Print(std::ostream& strm) const;
            void Print();

            double intersection( ray r ) const;
            Vector a1;
            Vector a2;
            Vector a3;
            Vector mNorm;
            double mD;
            
            Vector GetNormal(const Vector& point ) const;
    };

    class sphere : public primitive{
        public:
            inline void printon(bool print){mprint = print;}
            sphere();
            sphere(Vector c, double r);
            ~sphere();

            void Print(std::ostream& strm) const;
            void Print();


            double intersection( ray r ) const;
            Vector center;
            double radius;
            
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
            static double uniform_rand(double begin, double end); 
    };
}

#endif
