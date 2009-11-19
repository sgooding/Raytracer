//
// Author: Sean Gooding
// File: Vector.cpp
// Date: Tue Jan 27 20:41:53 MST 2009
// Purpose:
//          A little 3 vector class for project 1
//
//

#include "Vector.h"
#include <iostream>
#include <math.h>
#include <assert.h>

extern std::ostream& operator<<( std::ostream& strm,
        const smg::Vector& obj )
{
    obj.Print(strm);
    return strm;
}

namespace smg {

    Vector::Vector():
        x(0.0),
        y(0.0),
        z(0.0)
    {
    }

    Vector Vector::add ( const Vector a) const
    {
        Vector result;
        result.x = x+a.x;
        result.y = y+a.y;
        result.z = z+a.z;
        return result;
    }


    Vector Vector::subtract ( Vector a) const
    {
        Vector result;
        result.x = x-a.x;
        result.y = y-a.y;
        result.z = z-a.z;
        return result;
    }

    Vector Vector::operator-(Vector other) const
    {
        return subtract(other);
    }



    double Vector::dot ( Vector a ) const
    {
        return a.x*x+a.y*y+a.z*z;
    }

    Vector Vector::operator+(const Vector other) const
    {
        return add(other);
    }

    Vector Vector::cross( Vector a )
    {
        Vector result;
        result.x = y*a.z - z*a.y;
        result.y = z*a.x - x*a.z;
        result.z = x*a.y - y*a.x;
        return result;
    }

    Vector Vector::norm()
    {
        return Vector(x, y, z)/(mag());
    }

    Vector Vector::times( const Vector& other ) const
    {
        return Vector( x*other.x, y*other.y, z*other.z );
    }

    Vector Vector::operator*( const double scaler ) const
    {
        Vector result( scaler*x, 
                scaler*y, 
                scaler*z );
        return result;
    }

    Vector Vector::operator/( double scaler )
    {
        assert( scaler != 0.0 );

        Vector result( x/scaler, 
                y/scaler, 
                z/scaler );
        return result;
    }

    void
        Vector::Print( std::ostream& strm ) const
        {
            strm << x << ", " << y << ", " << z;
        }

    void
        Vector::Print( ) const
        {
            Print(std::cout);
        }

    double Vector::mag()
    {
        return sqrt( x*x + y*y + z*z );
    }

    Vector Vector::sphere2cart( double Theta, double Phi ) 
    {
        return Vector( sin(Theta)*cos(Phi),
                sin(Theta)*sin(Phi),
                cos(Theta) );
    }
}

