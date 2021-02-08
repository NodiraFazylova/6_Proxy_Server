#include <sstream>
#include <string>

#include "logger.h"
#include "cmd_parser.hpp"
#include "server.hpp"

#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include "tests.ipp"

int main( int argc, char ** argv )
{
    int result = Catch::Session().run( argc, argv ); /* run tests */

    return result;
}


TEST_CASE( "Test command line arguments parser:", "[cmd_parser]" )
{
    using namespace tests;

    proxy_server_6::server::config_t default_config;
    proxy_server_6::server::config_t config;
    bool help_opt = false;

    proxy_server_6::cmd_parser parser = proxy_server_6::make_cmd_parser(config, help_opt);

    SECTION( "empty argv:" )
    {
        char * argv[] = { "tests" };
        int    argc   = 1;       

        parser.parse( argc, argv );
        
        REQUIRE( help_opt == true );

        bool is_equal = config == default_config;
        REQUIRE( is_equal == true );
    }

    SECTION( "all invalid argv:" )
    {
        char * argv[] = { "tests", "qwerty", "qwerty" };
        int    argc = 3;

        parser.parse( argc, argv );

        REQUIRE( help_opt == true );

        bool is_equal = config == default_config;
        REQUIRE( is_equal == true );
    }

    SECTION( "one invalid argv:" )
    {
        char * argv[] = { "tests", "--ip=127.0.0.1", "--port=125", "qwerty" };
        int    argc = 4;

        parser.parse( argc, argv );

        REQUIRE( help_opt == true );

        bool is_equal = config == default_config;
        REQUIRE_FALSE( is_equal );
    }

    SECTION( "valid argv:" )
    {
        char * argv[] = { "tests", "--ip=127.0.0.1", "--port=125" };
        int    argc = 3;

        parser.parse( argc, argv );

        REQUIRE_FALSE( help_opt );

        bool is_equal = config == default_config;
        REQUIRE_FALSE( is_equal );
    }
}

TEST_CASE( "create server", "[server]" )
{
    proxy_server_6::server::config_t config;
    config.dir_path = "./"; // cur directory
    config.dir_path = "localhost";
    config.port = "124";
    // config.maxdate_size;  //default
    // config.workers_count; // default
    config.verbose = true; // enable logs

    proxy_server_6::server server( config );


}