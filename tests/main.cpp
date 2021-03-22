#include <random>
#include <sstream>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/post.hpp>
#include <boost/thread.hpp>

#include "cache.hpp"
#include "error.hpp"
#include "logger/logger.hpp"
#include "cmd_parser.hpp"
#include "protocol/protocol.hpp"
#include "server.hpp"
#include "utils/run_io_context.hpp"

#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include "tests.ipp"

int main( int argc, char ** argv )
{
    logger::init_console_logger();

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

        bool is_equal = ( config == default_config );
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
            auto command         = protocol::parse( unknown_protocol );

            //REQUIRE( command == invalid_command );  // todo not work
            REQUIRE( command.index() == invalid_command.index() );
        }

        SECTION( "command without params" )
        {
            // connect to
            {
                protocol::connect_request protocol; // protocol with empty params

                auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value
                auto command         = protocol::parse( protocol::make( protocol ) );

                //REQUIRE( command == invalid_command );  // todo not work
                REQUIRE( command.index() == invalid_command.index() );
            }

            // connect port
            {
                protocol::connect_response protocol; // protocol with empty params

                auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value
                auto command         = protocol::parse( protocol::make( protocol ) );

                //REQUIRE( command == invalid_command );  // todo not work
                REQUIRE( command.index() == invalid_command.index() );
            }

            // reconnetc to
            {
                protocol::reconnect_notify protocol; // protocol with empty params

                auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value
                auto command         = protocol::parse( protocol::make( protocol ) );

                //REQUIRE( command == invalid_command );  // todo not work
                REQUIRE( command.index() == invalid_command.index() );
            }

            // get file
            {
                protocol::get_file_request protocol; // protocol with empty params

                auto invalid_command = protocol::command_variant{}; // variant with std::monostate as value
                auto command         = protocol::parse( protocol::make( protocol ) );

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

            auto command  = protocol::parse( protocol::make( protocol ) );

            REQUIRE_NOTHROW( std::get<protocol::connect_request>( command ) );

            const auto & receive_protocol = std::get<protocol::connect_request>( command );
            
            REQUIRE( protocol.host == receive_protocol.host );
            REQUIRE( protocol.port == receive_protocol.port );
        }

        // connect port
        {
            protocol::connect_response protocol;
            protocol.port = 1234;

            auto command  = protocol::parse( protocol::make( protocol ) );

            REQUIRE_NOTHROW( std::get<protocol::connect_response>( command ) );

            const auto & receive_protocol = std::get<protocol::connect_response>( command );

            REQUIRE( protocol.port == receive_protocol.port );
        }

        // disconnect
        {
            protocol::disconnect_notify protocol;

            auto command = protocol::parse( protocol::make( protocol ) );

            REQUIRE_NOTHROW( std::get<protocol::disconnect_notify>( command ) );
        }

        // get list of cached files
        {
            protocol::get_cached_files_request protocol;

            auto command = protocol::parse( protocol::make( protocol ) );

            REQUIRE_NOTHROW( std::get<protocol::get_cached_files_request>( command ) );
        }

        // list of cached files (empty)
        {
            protocol::get_cached_files_response protocol;

            auto command = protocol::parse( protocol::make( protocol ) );

            REQUIRE_NOTHROW( std::get<protocol::get_cached_files_response>( command ) );
        }

        // list of cached files
        {
            protocol::get_cached_files_response protocol;
            for( auto i = 0; i < 10; ++i )
            {
                protocol.file_paths.emplace_back( std::to_string( i ) );
            }

            auto command = protocol::parse( protocol::make( protocol ) );

            REQUIRE_NOTHROW( std::get<protocol::get_cached_files_response>( command ) );

            const auto & receive_protocol = std::get<protocol::get_cached_files_response>( command );

            REQUIRE( protocol.file_paths == receive_protocol.file_paths );
        }

        // get file
        {
            protocol::get_file_request protocol;
            protocol.file_path = "file";

            auto command = protocol::parse( protocol::make( protocol ) );

            REQUIRE_NOTHROW( std::get<protocol::get_file_request>( command ) );

            const auto & received_protocol = std::get<protocol::get_file_request>( command );

            REQUIRE( protocol.file_path == received_protocol.file_path );
        }

        // file
        {
            protocol::get_file_response protocol;
            protocol.file_path = "file";

            auto command = protocol::parse( protocol::make( protocol ) );

            REQUIRE_NOTHROW( std::get<protocol::get_file_response>( command ) );

            const auto & received_protocol = std::get<protocol::get_file_response>( command );

            REQUIRE( protocol.file_path == received_protocol.file_path );
        }
    }
}

TEST_CASE( "cache unit test", "[cahce]" )
{
    // uncomment for enable logging
    proxy_server_6::cache cache( 5, 1024, true );

    // and comment this
    // proxy_server_6::cache cache();

    SECTION( "insert/get file in single thread " )
    {
        const size_t min_filename_len = 10;
        const size_t max_filename_len = 40;

        const size_t min_file_len = 100;
        const size_t max_file_len = 528;

        const size_t files_count = 100;

        std::vector<std::string> filenames( tests::generate_file_or_filenames( files_count, min_filename_len, max_filename_len ) );
        std::vector<std::string> files( tests::generate_file_or_filenames( files_count, min_file_len, max_file_len ) );

        tests::files cached_files;
        for( size_t i = 0, n = files_count * 4; i < n; ++i )
        {
            tests::insert_file( filenames[i % files_count], files[i % files_count], cache );
            tests::get_file( filenames[i % files_count], cache, cached_files );
        }

        for( auto i = 0; i < cached_files.files.size(); ++i )
        {
            REQUIRE( files[i % files_count] == cached_files.files[i] );
        }
    }

    SECTION( "insert/get files in multithreading" )
    {
        boost::asio::io_context io_context;

        const size_t min_filename_len = 10;
        const size_t max_filename_len = 40;

        const size_t min_file_len = 100;
        const size_t max_file_len = 528;

        const size_t files_count = 100;

        const size_t workers_count = 4;

        std::vector<std::string> filenames( tests::generate_file_or_filenames( files_count, min_filename_len, max_filename_len ) );
        std::vector<std::string> files( tests::generate_file_or_filenames( files_count, min_file_len, max_file_len ) );

        boost::thread_group workers;
        for( int64_t i = 0; i < workers_count; ++i )
        {
            workers.create_thread( proxy_server_6::io_run_context_once( io_context ) );
        }

        io_context.run();
        std::uniform_int_distribution<size_t>  uni( 0, 1 );  /**< guaranteed unbiased */
        tests::files cached_files;
        for( size_t i = 0, n = files_count * 4; i < n; ++i )
        {
            bool inserting = tests::rand( uni );
            if( inserting || !i )
            {
                boost::asio::post( std::bind(&tests::insert_file, std::cref( filenames[i % files_count] ), std::cref( files[i % files_count] ), std::ref( cache )) );
            }
            else
            {
                boost::asio::post( std::bind( &tests::get_file, std::cref( filenames[(i - 1) % files_count] ), std::cref( cache ), std::ref( cached_files ) ) );
            }
        }

        io_context.stop();
        workers.join_all();
        
        // not work without this
        //using namespace std::chrono_literals;
        //std::this_thread::sleep_for( 1s );

        // in this case success is not abort program ))
    }

    SECTION( "getting all cached files" )
    {
        /*
        * since we can't guarantee the order in which the files will be inserted in multithreaded mode, we execute several inserts in one thread and then get all the cached files
        */
        const size_t min_filename_len = 10;
        const size_t max_filename_len = 15;

        const size_t min_file_len = 40;
        const size_t max_file_len = 100;

        const size_t files_count = 30;

        const size_t workers_count = 4;

        std::vector<std::string> filenames( tests::generate_file_or_filenames( files_count, min_filename_len, max_filename_len ) );
        std::vector<std::string> files( tests::generate_file_or_filenames( files_count, min_file_len, max_file_len ) );

        for( auto i = 0; i < files_count; ++i )
        {
            tests::insert_file( filenames[i], files[i], cache );
        }

        error::errc errc;
        std::vector<std::string> cached_files( cache.get_cached_files(errc) );

        //REQUIRE( errc == error::errc::not_error );

        // we can check only that each cached file has in pregenerated files
        for( auto i = 0; i < cached_files.size(); ++i )
        {
            bool is_in_files = std::find( std::begin( files ), std::end( files ), cached_files[i] ) == std::end( files );
            REQUIRE_FALSE( is_in_files );
        }
    }
}

TEST_CASE( "create server", "[server]" )
{
    logger::deinit_logger(); // init log in server

    proxy_server_6::server::config_t config;
    config.dir_path = "./"; // cur directory
    config.dir_path = "localhost";
    config.port     = "124";
    // config.maxdate_size;  //default
    // config.workers_count; // default
    //config.verbose = true; // enable logs

    proxy_server_6::server server( config );


}