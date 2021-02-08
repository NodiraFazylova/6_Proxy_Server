#include <future>
#include <mutex>
#include <map>
#include <vector>
#include <utility>

template <typename T>
T abs( T x )
{
    return x < 0 ? -x : x;
}

auto lock( std::mutex & m )
{
    return std::lock_guard<mutex>{ m };
}

template <typename K, typename V>
class cache
{
public:
    static_assert(
        std::is_convertible_v<K, uint64_t>,
        "ConcurrentMap supports only integer keys"
        );

    struct access
    {
        std::lock_guard<std::mutex> guard;
        V & ref_to_value;

        access( const K & key, std::pair<std::mutex, map<K, V>> & bucket_content )
            : guard( bucket_content.first )
            , ref_to_value( bucket_content.second[key] )
        {}
    };

    explicit cache( size_t bucket_count )
        : data( bucket_count )
    {}

    access operator[]( const K & key )
    {
        auto & bucket = data[abs( key ) % data.size()];
        return { key, bucket };
    }

    std::map<K, V> build_ordinary_map()
    {
        std::map<K, V> result;
        for( auto & [mtx, mapping] : data )
        {
            auto g = lock( mtx );
            result.insert( std::begin( mapping ), std::end( mapping ) );
        }
        return result;
    }

private:
    std::vector<std::pair<std::mutex, std::map<K, V>>> data;
};
