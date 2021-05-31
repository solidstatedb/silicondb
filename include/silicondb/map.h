#ifndef SILICONDB_MAP
#define SILICONDB_MAP

#include <functional>
#include <list>

#include <shared_mutex>
#include <mutex>

namespace silicondb
{
    // map: threadsafe map implemented as hash table.
    template <typename Key, typename Value, typename Hash = std::hash<Key>>
    class map
    {
    private:
        class bucket_t
        {
        private:
            typedef std::pair<Key, Value> bucket_value_t;
            typedef std::list<bucket_value_t> bucket_data_t;

            bucket_data_t data;
            mutable std::shared_mutex mutex;
        };
    };

};

#endif