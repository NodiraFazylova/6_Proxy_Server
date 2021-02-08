#include <sstream>
#include <string>

#include "logger.h"
#include "cmd_parser.hpp"
#include "protocol/protocol.hpp"
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


TEST_CASE( "protocol unit test", "[protocol]" )
{
    SECTION( "invalid protocols" )
    {
        SECTION( "empty protocol" )
        {
            std::string empty_protocol;

            auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value
            auto command         = protocol::parse( empty_protocol );
            
            //REQUIRE( command == invalid_command );  // todo not work
            REQUIRE( command.index() == invalid_command.index() );
        }

        SECTION( "unknown command" )
        {
            std::string unknown_protocol{"qwerty 1234:123 32"};

            auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value
            auto command = protocol::parse( unknown_protocol );

            //REQUIRE( command == invalid_command );  // todo not work
            REQUIRE( command.index() == invalid_command.index() );
        }

        SECTION( "command without params" )
        {
            // connect to
            {
                protocol::connect_request protocol; // protocol with empty params
LOG_DEBUG( std::cout, "\"" << protocol::make( protocol ) << "\"" );
                auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value
                auto command = protocol::parse(protocol::make( protocol));

                //REQUIRE( command == invalid_command );  // todo not work
                REQUIRE( command.index() == invalid_command.index() );
            }

            // reconnetc to
            {
                protocol::reconnect_notify protocol; // protocol with empty params
LOG_DEBUG( std::cout, "\"" << protocol::make( protocol ) << "\"" );
                auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value
                auto command = protocol::parse( protocol::make( protocol ) );

                //REQUIRE( command == invalid_command );  // todo not work
                REQUIRE( command.index() == invalid_command.index() );
            }

            // get file
            {
                protocol::get_file_request protocol; // protocol with empty params
 LOG_DEBUG( std::cout, "\"" << protocol::make( protocol ) << "\"" );
                auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value
                auto command = protocol::parse( protocol::make( protocol ) );

                //REQUIRE( command == invalid_command );  // todo not work
                REQUIRE( command.index() == invalid_command.index() );
            }
        }

        SECTION( "command with invalid params" )
        {
            // connect request
            {
                std::string protocol{ "aaaa" /* fail in socket level, now - ok*/  "aaaaqwweqw" /* stoi in parse thow exception */ };
                
                protocol::command_variant command;
                auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value

                REQUIRE_NOTHROW( [&] () { command = protocol::parse( protocol );  } ); // parse should not throw exceptions
                REQUIRE( command.index() == invalid_command.index() );
            }

            // reconnect request
            {
                protocol::reconnect_notify protocol;
                protocol.host = "127.0.0.1";

                protocol::command_variant command;
                auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value

                REQUIRE_NOTHROW( [&] () { command = protocol::parse( protocol::make( protocol ) );  } ); // parse should not throw exceptions
                REQUIRE( command.index() == invalid_command.index() );
            }
        }
    }

    SECTION( "valid protocols" )
    {
        // connect to
        {
            protocol::connect_request protocol;
            protocol.host = "127.0.0.1";
            protocol.port = 123;

            auto command = protocol::parse( protocol::make( protocol ) );
            const auto & receive_protocol = std::get<protocol::connect_request>( command );
            
            REQUIRE( protocol.host == receive_protocol.host );
            REQUIRE( protocol.port == receive_protocol.port );
        }
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