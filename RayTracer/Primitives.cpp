//
// Author: Sean Gooding
// File: Primitives.cpp
// Date: Tue Jan 27 20:41:53 MST 2009
// Purpose:
//          A few ray tracing primitives
//
//

#include "Primitives.h"
#include <math.h>
#include <assert.h>
#include <fstream>
#include <sstream>

#define VERY_SMALL_NUMBER 0.0000001

namespace smg
{
   int get_unique_name()
   {
       static int index = 0;
       return index++;
   }

   Vector ray::get_endpoint(const float t) const
   {
       return Vector(origin + direction*t);
   }

    primitive::primitive() :
        m()
    {
    }

    void primitive::compute_probs()
    {
        if( m.mirror )
        {
            m.prob.absorption = 0.0;
            m.prob.diffuse = 0.0;
            m.prob.specular = 1.0;
            m.prob.transmission = 0.0;
        }
        else if( m.glass )
        {
            m.prob.absorption = 0.0;
            m.prob.diffuse = 0.2;
            m.prob.specular = 0.3;
            m.prob.transmission = 0.5;
        } else 
        {
            m.prob.absorption = 0.3;
            m.prob.diffuse = 0.3;
            m.prob.specular = 0.3;
            m.prob.transmission = 0.0;
        }

        m.prob.total = m.prob.absorption + m.prob.diffuse + m.prob.specular + m.prob.transmission;
        m.prob.absorption /=m.prob.total;
        m.prob.diffuse /= m.prob.total;
        m.prob.specular /= m.prob.total;
        m.prob.transmission /= m.prob.total;

    }

    material_prob::type primitive::prob_test( float element )
    {
        if( element <= m.prob.absorption)
        {
            return material_prob::ABSORPTION;
        } else if ( element < m.prob.absorption + m.prob.diffuse )
        {
            return material_prob::DIFFUSE_REFLECTION;
        } else if ( element < m.prob.absorption + m.prob.diffuse + m.prob.specular)
        {
            return material_prob::SPECULAR_REFLECTION;
        } else if (  element < m.prob.absorption + m.prob.diffuse + m.prob.specular + m.prob.transmission)
        {
            return material_prob::TRANSMISSION;
        }

        return material_prob::UNKNOWN;
    }


    void primitive::scale_power( material_prob::type transmission_type, Vector& power_mod )
    {
        assert(0);
        switch (transmission_type)
        {
            case material_prob::DIFFUSE_REFLECTION:
                {
                    power_mod = power_mod.times( power_mod );
                    break;
                }
            case material_prob::SPECULAR_REFLECTION:
                {
                    power_mod = power_mod.times( power_mod );
                    break;
                }
            default:
                std::cout << "Currently only reduces power for DIFFUSE and SPECULAR REFLECTION" << std::endl;
        }
    }

    // Compute the diffuse 
    Vector primitive::compute_brdf_r( const Vector& position, 
                                      const Vector& incident, 
                                      const Vector& reflected ) const
    {
        Vector Normal( GetNormal( position ) );
        Vector L( reflected*(-1.0) );
        Vector H( (incident+L).norm() );
        float Is( m.k_spec*pow( H.dot(Normal), m.n_spec) );

        float normal_2_ls( Normal.dot( L ) );
        return Vector(  m.k_diff_R*( normal_2_ls ) + Is,
                        m.k_diff_G*( normal_2_ls ) + Is,
                        m.k_diff_B*( normal_2_ls ) + Is );

    }

    // Specular Reflection
    Vector primitive::compute_brdf_s( const Vector& position, 
                                      const Vector& incident, 
                                      const Vector& reflected ) const
    {
 
        Vector Normal( GetNormal( position ) );
        Vector L( reflected*(-1.0) );
        Vector H( (incident+L).norm() );

        // Is Term
        return Vector( m.k_spec*pow( H.dot(Normal), m.n_spec), 
                       m.k_spec*pow( H.dot(Normal), m.n_spec), 
                       m.k_spec*pow( H.dot(Normal), m.n_spec) ); 
    }
    Vector primitive::compute_brdf_d( const Vector& position, 
                                      const Vector& incident, 
                                      const Vector& reflected ) const
    {
        Vector Normal( GetNormal( position ) );
        Vector L( reflected*(-1.0) );
        Vector H( (incident+L).norm() );

        float normal_2_ls( Normal.dot( L ) );
        return Vector(  m.k_diff_R*( normal_2_ls ), // Idr 
                        m.k_diff_G*( normal_2_ls ), // Idg 
                        m.k_diff_B*( normal_2_ls ) ); // Idb
    }



    primitive::~primitive()
    {
    }


    void primitive::Print(std::ostream& strm) const
    {
    }


    void primitive::Print() const
    {
        Print(std::cout);
    }


    triangle::triangle() :
        a1(),
        a2(),
        a3(),
        mNorm(),
        mD(0)
    {
        std::stringstream ss; ss << get_unique_name() << "_Triangle";
        name = ss.str();
    }

    triangle::triangle(Vector a, Vector b, Vector c, Vector view):
        a1(a),
        a2(b),
        a3(c),
        mNorm(),
        mD(0)
    {
        std::stringstream ss; ss << get_unique_name() << "_Triangle";
        name = ss.str();
        Vector a1a2( a2-a1 );
        Vector a1a3( a3-a1 );
        Vector N( a1a2.cross(a1a3) );

        mNorm = N.norm();
        mD = -mNorm.dot(a1);
    }

    triangle::~triangle()
    {
    }

    float triangle::intersection(ray r) const
    {
        float den = r.direction.dot(mNorm);

        float t = -( r.origin.dot( mNorm ) + mD );

        if( fabs(den) < VERY_SMALL_NUMBER )
        {
            den = VERY_SMALL_NUMBER;
        }

        t /= den;

        if( fabs(t) < VERY_SMALL_NUMBER )
        {
            return -1;
            assert(0);
        }

        // Class Version
        Vector P( r.origin + r.direction*t );

        Vector pa1( a1 - P );
        Vector pa2( a2 - P );
        Vector pa3( a3 - P );

        Vector v12( pa1.cross(pa2) );
        Vector v23( pa2.cross(pa3) );
        Vector v31( pa3.cross(pa1) );

        // Exists a degenerate case if point 
        // is aligned with 2 other points.
        float v12m = v12.mag();
        if( fabs( v12m ) < VERY_SMALL_NUMBER )
        {
            if( pa1.dot(pa2) > VERY_SMALL_NUMBER  )
            {
                return -1.0;
            } else
            {
                return t;
            }
        }
        
        float v23m = v23.mag();
        if( fabs( v23m ) < VERY_SMALL_NUMBER  )
        {
            if( pa2.dot(pa3) > VERY_SMALL_NUMBER )
            {
                return -1.0;
            } else
            {
                return t;
            }
        }

        float v31m = v31.mag();
        if( fabs( v31m ) < VERY_SMALL_NUMBER  )
        {
            if( pa3.dot(pa1) > VERY_SMALL_NUMBER )
            {
                return -1.0;
            } else
            {
                return t;
            }
        }

        if( ( v12.dot(v23) ) < VERY_SMALL_NUMBER )
        {
            return -1;
        }

        if( ( v12.dot(v31)  ) < VERY_SMALL_NUMBER )
        {
            return -1;
        }
        
        if( v23.dot( v31 ) < VERY_SMALL_NUMBER )
        {
            return -1;
        }

        return t;
    }

    void triangle::Print(std::ostream& strm) const
    {
        strm << "Name: " << name << std::endl;
        strm << "a1:   " << a1 << std::endl;
        strm << "a2:   " << a2 << std::endl;
        strm << "a3:   " << a3 << std::endl;
    }


    void triangle::Print()
    {
        Print(std::cout);
    }


    sphere::sphere() :
        center(),
        radius(0.0),
        mprint(false)
    {
        std::stringstream ss; ss << get_unique_name() << "_Sphere";
        name = ss.str();
    }
    
    // The "view" determines what side of the
    // plane the interest point is
    // not sure what will happen if the side 
    // is on the backside
    Vector triangle::GetNormal(const Vector& none ) const
    {
        return mNorm;
    }

    sphere::sphere(Vector c, float r):
        center(c),
        radius(r),
        mprint(false)
    {
        std::stringstream ss; ss << get_unique_name() << "_Sphere";
        name = ss.str();
    }

    sphere::~sphere()
    {
    }

    void sphere::Print(std::ostream& strm) const
    {
        strm << "Name  : " << name << std::endl;
        strm << "Glass : " << m.glass << std::endl;
        strm << "Mirror: " << m.mirror << std::endl;
        strm << "Center: " << center << std::endl;
        strm << "Radius: " << radius << std::endl;
    }


    void sphere::Print()
    {
        Print(std::cout);
    }

    float sphere::intersection(ray r) const
    {
        Vector co( r.origin - center );
        float co_m( co.mag() );
        float C = co_m*co_m - radius*radius;
        
        float B = co.dot(r.direction);
        B *= 2.0;

        float A = r.direction.mag();
        A *= A;

        float delta( B*B - 4.0*A*C );
        if( delta > 0.0 )
        {
            float t1 = (-B + sqrt( delta ) )/ (2*A); 
            float t2 = (-B - sqrt( delta ) )/ (2*A); 

            if( t1 < 0.0 && t2 < 0.0 )
            {
                // if all are negative no intersection
                return -1;
            } else if( t1 > 0.0 && t2 > 0.0 ) 
            {
                // if both are positive discard larger one
                return t1 > t2? t2 : t1;
            } else if( t1 > 0.0 && t2 < 0.0 )
            {
                // if t2 is negative return t1
                return t1;
            } else 
            {
                // if t1 is negative return t2
                return t2;
            }
        } else if( fabs( delta ) < VERY_SMALL_NUMBER )
        {
            float t(-B/(2.0*A));
            return t;
        }        

        return -1;
    }


    Vector sphere::GetNormal(const Vector& point) const
    {
        Vector cp( point - center );
        float mag( cp.mag() );
        if( mag < VERY_SMALL_NUMBER )
        {
           assert(0);
        }
        return cp * (1.0/mag);
    }


    plane::plane() :
        norm(),
        nnorm(),
        d(0)
    {
    }

    plane::plane(Vector a, float b):
        norm(a),
        d(b),
        nnorm()
    {
        nnorm = norm*(-1.0);
    }

    plane::~plane()
    {
    }

    float plane::intersection(ray r) const
    {
        float den = r.direction.dot( norm );
        if( fabs( den ) <= VERY_SMALL_NUMBER )
        {
            return -1.0;
        }

        float t = norm.dot( r.origin );
        t += d;
        t *= -1.0;
        t /= den;

        if( t >= 0.0 )
        {
            return t; 
        }
        return -1.0;
    }

    void plane::Print(std::ostream& strm) const
    {
        strm << "norm: " << norm << std::endl;
        strm << "d: " << d << std::endl;
    }

    Vector plane::GetNormal(const Vector& point ) const
    {
        return norm;
    }

    void plane::Print()
    {
        Print(std::cout);
    }



    void 
    SchlickBRDF::ComputeBRDF( const Vector& Normal,
            const Vector& Win,
            const Vector& Wout )
    {
        Vector Wsum( Win + Wout );
        Vector H( Wsum.norm() );

        float u( Win.dot( H ) );
        float t( Normal.dot( H ) );

        float v( Win.dot( Normal ) );
        float vout( Wout.dot( Normal ) ) ;


        //Vector T( 0.0, 0.0, 0.0 ); // Scratch Direction
        //Vector w; 

        float eta_1 = General::uniform_rand( 0.0, 1.0 );
        float eta_2 = General::uniform_rand( 0.0, 1.0 );

        float Psi = 0.0;
        float sigma = 0.0;
        float eta_2_2 = eta_2*eta_2;
        float Psi_2 = Psi*Psi;

        // Glossy Reflected Direction Terms
        float t_g = sqrt( eta_1/ ( sigma - eta_1*sigma + eta_1 ) );
        float w = cos( M_PI/2.0*sqrt( Psi_2*eta_2_2/
                    ( 1.0 - eta_2_2 + eta_2_2*Psi_2) ) );


    }

    float General::uniform_rand(float begin, float end)
    {
        return ( begin + ( (end-begin) * (rand() / (RAND_MAX + 1.0))) );
    }


}
