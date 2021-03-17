namespace tests
{

bool operator== ( const proxy_server_6::server::config_t & lh, const proxy_server_6::server::config_t & rh )
{
    return  lh.host == rh.host &&
        lh.port == rh.port &&
        lh.dir_path == rh.dir_path &&
        lh.maxdata_size == rh.maxdata_size &&
        lh.workers_count == rh.workers_count &&
        lh.verbose == rh.verbose;
}

size_t rand( std::uniform_int_distribution<size_t> & distr )
{
    std::random_device                      rd;   /**< only used once to initialise (seed) engine */
    std::mt19937                            rng( rd() );  /**< random-number engine used (Mersenne-Twister in this case) */

    return distr( rng );
}

std::string random_string( size_t length )
{
    auto randchar = [] () -> char
    {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        
        std::uniform_int_distribution<size_t>  uni(0, sizeof( charset ) /* 62 */);  /**< guaranteed unbiased */

        return charset[rand(uni)];
    };

    std::string str( length, 0 );
    std::generate_n( str.begin(), length, randchar );
    return str;
}


struct files
{
    std::mutex mtx;
    std::vector<std::string> files;
};


void insert_file( const std::string & filename, const std::string & file, proxy_server_6::cache & cache )
{
    error::errc errc;
    cache.insert_file( filename, file, errc );
}


void get_file( const std::string & filename, const proxy_server_6::cache & cache, files & buffer )
{
    std::lock_guard lock( buffer.mtx );
    error::errc errc;
    buffer.files.emplace_back( cache.get_file( filename, errc ) );
}


std::vector<std::string> generate_file_or_filenames( const size_t count, const size_t min_len, const size_t max_len )
{
    std::uniform_int_distribution<size_t>  uni( min_len, max_len );  /**< guaranteed unbiased */
    std::vector<std::string> content;

    for( auto i = 0; i < count; ++i )
    {
        content.emplace_back( random_string( rand( uni ) ) );
    }

    return content;
}

}   // namespace tests