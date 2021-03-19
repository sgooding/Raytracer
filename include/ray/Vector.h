//
// Author: Sean Gooding
// File: Vector.h
// Date: Tue Jan 27 20:41:53 MST 2009
// Purpose:
//          A little 3 vector class for project 1
//
//
#ifndef smg_Vector_H_
#define smg_Vector_H_

#include <iostream>

namespace smg
{

    class Vector
    {
    public:
        float x, y, z;
        Vector();

        Vector(const Vector &other) : x(other.x),
                                      y(other.y),
                                      z(other.z) {}

        inline Vector(float ix, float iy, float iz) : x(ix),
                                                      y(iy),
                                                      z(iz) {}

        Vector add(const Vector &a) const;
        Vector subtract(const Vector &a) const;
        float dot(const Vector &a) const;
        Vector cross(const Vector &a) const;
        float mag();
        Vector norm();
        Vector times(const Vector &other) const;

        Vector operator+(const Vector &other) const;
        Vector operator-(const Vector &other) const;
        Vector operator*(const float &scaler) const;
        Vector operator/(const float &scaler);

        static Vector sphere2cart(const float &Theta, const float &Phi);

        void Print(std::ostream &ofs) const;
        void Print() const;
    };

}

extern std::ostream &operator<<(std::ostream &strm,
                                const smg::Vector &obj);

#endif
