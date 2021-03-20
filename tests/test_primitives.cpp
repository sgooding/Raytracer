
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <ray/Vector.h>
#include <cmath>
using namespace smg;

TEST_CASE("Test Constructor", "[primitive]")
{
    float x(.3);
    float y(.4);
    float z(.5);
    Vector test_vector(x, y, z);
    REQUIRE(x == test_vector.x);
    REQUIRE(y == test_vector.y);
    REQUIRE(z == test_vector.z);
}

TEST_CASE("Test Copy", "[primitive]")
{
    float x(.3);
    float y(.4);
    float z(.5);
    Vector test_vector(x, y, z);
    REQUIRE(x == test_vector.x);
    REQUIRE(y == test_vector.y);
    REQUIRE(z == test_vector.z);

    Vector test_vector_2(test_vector);
    REQUIRE(x == test_vector_2.x);
    REQUIRE(y == test_vector_2.y);
    REQUIRE(z == test_vector_2.z);
}

TEST_CASE("Test Add", "[primitive]")
{
    float x(.3);
    float y(.4);
    float z(.5);
    Vector A(x, y, z);
    Vector B(A);
    Vector C(B.add(A));

    REQUIRE((x + x) == C.x);
    REQUIRE((y + y) == C.y);
    REQUIRE((z + z) == C.z);
}

TEST_CASE("Test Subtract", "[primitive]")
{
    float x(.3);
    float y(.4);
    float z(.5);
    Vector A(x, y, z);
    Vector B(A);
    Vector C(B.subtract(A));

    REQUIRE((x - x) == C.x);
    REQUIRE((y - y) == C.y);
    REQUIRE((z - z) == C.z);
}

TEST_CASE("Test Dot", "[primitive]")
{
    float x(.3);
    float y(.4);
    float z(.5);
    float dot = x*x+y*y+z*z;
    Vector A(x, y, z);
    Vector B(A);

    REQUIRE(dot == B.dot(A));
}

TEST_CASE("Test Mag", "[primitive]")
{
    float x(.3);
    float y(.4);
    float z(.5);
    float dot = x*x+y*y+z*z;
    float mag = std::sqrt(dot);
    Vector A(x, y, z);

    REQUIRE(mag == A.mag());
}

TEST_CASE("Test Cross", "[primitive]")
{
    Vector A(2., -3., 1.);
    Vector B(-2., 1., 1.);
    Vector C(A.cross(B));
    REQUIRE(C.x == -4.0);
    REQUIRE(C.y == -4.0);
    REQUIRE(C.z == -4.0);
}