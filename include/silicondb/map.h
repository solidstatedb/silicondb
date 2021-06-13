/// \file

#ifndef SILICONDB_MAP
#define SILICONDB_MAP

#include <functional>
#include <vector>
#include <memory>

#include <silicondb/list.h>

namespace silicondb
{
    /// \class map map.h silicondb/map.h
    /// Threadsafe map implemented as lookup table.
    /// Delegates all locking to silicondb::list
    template <typename Key, typename Value, typename Hash = std::hash<Key>>
    class map
    {
    private:
        /// map::bucket: hash bucket within the hash table, for storing
        /// entries with clashing hash values.
        class bucket
        {
        private:
            typedef std::pair<Key, Value> bucket_value;

            list<bucket_value> data;

            /// map::bucket::find_entry_for: returns a iterator to the first
            /// (key, value) pair within this buckets data list with given
            /// argument key.
            std::shared_ptr<bucket_value> find_entry_for(Key const &key)
            {
                return data.find_first_if([&](bucket_value const &item)
                                          { return item.first == key; });
            }

        public:
            /// map::bucket::get: returns the value mapped to the given key
            /// if present, the default_value passed otherwise
            Value get(Key const &key, Value const &default_value)
            {
                auto const entry = find_entry_for(key);
                return entry ? entry->second : default_value;
            }

            /// map::bucket::contains: returns true if this key has a value
            /// associated with it, and that entry is stored in this bucket
            bool contains(Key const &key)
            {
                auto const entry = find_entry_for(key);
                return bool(entry);
            }

            /// map::bucket::put: establishes a key value mapping for the
            /// key value pair. Creates a new key value pair in the bucket
            /// data if the mapping didn't exist before. Otherwise, it
            /// overwrites the old mapping with the new value.
            void put(Key const &key, Value const &value)
            {
                auto const entry = find_entry_for(key);
                if (entry)
                    entry->second = value;
                else
                    data.push_front(bucket_value(key, value));
            }

            /// map::bucket::unmap: erases the value associated with this
            /// key, if present, from the bucket data.
            void unmap(Key const &key)
            {
                data.remove_if([&](bucket_value const &item)
                               { return item.first == key; });
            }
        };

        std::vector<std::unique_ptr<bucket>> buckets;
        Hash hasher;

        /// Returns the bucket mapped to the given key's hash value. Since
        /// this is a read-only operation no locking is involved.
        bucket &get_bucket(Key const &key) const
        {
            std::size_t const bucket_index = hasher(key) % buckets.size();
            return *buckets[bucket_index];
        }

    public:
        /// Creates a map with the given number of buckets for the lookup table
        /// and the hashing function to be used for hashing keys.
        map(unsigned int number_of_buckets = 19, Hash hasher_ = Hash())
            : buckets(number_of_buckets), hasher(hasher_)
        {
            for (int i{0}; i != number_of_buckets; ++i)
                buckets[i].reset(new bucket);
        }

        map(map const &other) = delete;
        map &operator=(map const &other) = delete;

        /// Returns the value associated with the given key. If no value is
        /// associated, the default value passed is returned.
        Value get(Key const &key, Value const &default_value = Value()) const
        {
            return get_bucket(key).get(key, default_value);
        }

        /// Returns whether this map contains a mapping for the given key. It
        /// is to be used in scenario's when a default mapped value is not obvious.
        bool contains(Key const &key) { return get_bucket(key).contains(key); }

        /// Puts the value in this map. If no value is already mapped with the given
        /// key, a new (Key, Value) entry is created to associate the given Key with
        /// the given Value. If a mapping already exists for the given key, the
        /// value entry is updated.
        void put(Key const &key, Value const &value) { get_bucket(key).put(key, value); }

        /// Unmaps the value associated with the given key. Removes the (Key, Value)
        /// entry from this map, corresponding to the given key.
        void unmap(Key const &key) { get_bucket(key).unmap(key); }
    };

};

#endif