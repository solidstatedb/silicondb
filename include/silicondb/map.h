#ifndef SILICONDB_MAP
#define SILICONDB_MAP

#include <functional>
#include <vector>
#include <list>

#include <memory>

#include <shared_mutex>
#include <mutex>

namespace silicondb
{
    // map: threadsafe map implemented as hash table.
    template <typename Key, typename Value, typename Hash = std::hash<Key>>
    class map
    {
    private:
        // map::bucket: hash bucket within the hash table, for storing
        // entries with clashing hash values.
        class bucket
        {
        private:
            typedef std::pair<Key, Value> bucket_value;

            typedef std::list<bucket_value> bucket_data;
            typedef typename bucket_data::iterator bucket_iterator;

            bucket_data data;
            mutable std::shared_mutex mutex; // multi-read, single-write

            // map::bucket::find_entry_for: returns a iterator to the first
            // (key, value) pair within this buckets data list with given
            // argument key.
            bucket_iterator find_entry_for(Key const &key)
            {
                return std::find_if(data.begin(), data.end(),
                                    [&](bucket_value const &item)
                                    { return item.first == key; });
            }

        public:
            // map::bucket::get: returns the value mapped to the given key
            // if present, the default_value passed otherwise
            Value get(Key const &key, Value const &default_value)
            {
                std::shared_lock<std::shared_mutex> lock(mutex);
                bucket_iterator const entry = find_entry_for(key);
                return entry == data.end() ? default_value : entry->second;
            }

            // map::bucket::put: establishes a key value mapping for the
            // key value pair. Creates a new key value pair in the bucket
            // data if the mapping didn't exist before. Otherwise, it
            // overwrites the old mapping with the new value.
            void put(Key const &key, Value const &value)
            {
                std::shared_lock<std::shared_mutex> lock(mutex);
                bucket_iterator const entry = find_entry_for(key);

                if (entry == data.end())
                    data.push_back(bucket_value(key, value));
                else
                    entry->second = value;
            }

            // map::bucket::unmap: erases the value associated with this
            // key, if present, from the bucket data.
            void unmap(Key const &key)
            {
                std::shared_lock<std::shared_mutex> lock(mutex);
                bucket_iterator const entry = find_entry_for(key);

                if (entry != data.end())
                    data.erase(entry);
            }
        };

        std::vector<std::unique_ptr<bucket>> buckets;
        Hash hasher;

        bucket &get_bucket(Key const &key) const
        {
            std::size_t const bucket_index = hasher(key) % buckets.size();
            return *buckets[bucket_index];
        }

    public:
        map(unsigned int number_of_buckets = 19, Hash hasher_ = Hash())
            : buckets(number_of_buckets), hasher(hasher_)
        {
            for (int i{0}; i != number_of_buckets; ++i)
            {
                buckets[i].reset(new bucket);
            }
        }

        map(map const &other) = delete;
        map &operator=(map const &other) = delete;

        Value get(Key const &key, Value const &default_value = Value()) const
        {
            return get_bucket(key).get(key, default_value);
        }

        void put(Key const &key, Value const &value)
        {
            get_bucket(key).put(key, value);
        }

        void unmap(Key const &key)
        {
            get_bucket(key).unmap(key);
        }
    };

};

#endif