#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE( "Quick check", "[main]" ) {
    REQUIRE( true );
}

TEST_CASE( "Quick check 2", "[main]" ) {
    REQUIRE( true );
}

SCENARIO( "Quick check 3", "[ANN]" ) {
    REQUIRE( false );
}

