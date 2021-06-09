#include <silicondb/map.h>
#include <gtest/gtest.h>

#include <thread>
#include <chrono>

template <typename integer_map>
void test_map_consistency_serial(integer_map &map)
{
    for (int i = 0; i < 10; i++)
        map.put(i, i + 1);

    for (int i = 0; i < 10; i++)
        EXPECT_EQ(map.get(i), i + 1);
}

template <typename integer_map>
void test_map_unmap_serial(integer_map &map)
{
    for (int i = 1; i <= 10; i++)
        map.put(i, i);

    for (int i = 1; i <= 10; i++)
    {
        if (i % 2 == 0)
            map.unmap(i);
    }

    for (int i = 1; i <= 10; i++)
    {
        if (i % 3 == 0)
            map.unmap(i);
    }

    for (int i = 1; i <= 10; i++)
    {
        if (i % 2 == 0 || i % 3 == 0)
        {
            EXPECT_EQ(map.contains(i), false);
        }
        else
        {
            EXPECT_EQ(map.contains(i), true);
            EXPECT_EQ(map.get(i), i);
        }
    }

    for (int i = 11; i <= 15; i++)
        EXPECT_EQ(map.contains(i), false);
}

template <typename integer_map>
void test_map_unmap(integer_map &map)
{
    using namespace std::chrono_literals;

    // random sleep durations
    auto d1 = 29ms, d2 = 53ms;

    for (int i = 1; i <= 10; i++)
        map.put(i, i);

    std::thread unmap_thread_1([&]()
                               {
                                   for (int i = 1; i <= 10; i++)
                                   {
                                       std::this_thread::sleep_for(d1);
                                       if (i % 2 == 0)
                                           map.unmap(i);
                                   }
                               });

    std::thread unmap_thread_2([&]()
                               {
                                   for (int i = 1; i <= 10; i++)
                                   {
                                       std::this_thread::sleep_for(d2);
                                       if (i % 3 == 0)
                                           map.unmap(i);
                                   }
                               });

    unmap_thread_1.join();
    unmap_thread_2.join();

    auto read_thread_task = [&]()
    {
        for (int i = 1; i <= 10; i++)
        {
            if (i % 2 == 0 || i % 3 == 0)
            {
                std::this_thread::sleep_for(d2);
                EXPECT_EQ(map.contains(i), false);
            }
            else
            {
                std::this_thread::sleep_for(d1);
                EXPECT_EQ(map.contains(i), true);
                EXPECT_EQ(map.get(i), i);
            }
        }

        for (int i = 11; i <= 15; i++)
            EXPECT_EQ(map.contains(i), false);
    };

    std::thread reader_thread_1(read_thread_task);
    std::thread reader_thread_2(read_thread_task);

    reader_thread_1.join();
    reader_thread_2.join();
}

template <typename integer_map>
void test_map_consistency(integer_map &map)
{
    using namespace std::chrono_literals;

    // random sleep durations
    auto d1 = 276ms, d2 = 532ms;

    std::thread put_thread_1([&]()
                             {
                                 map.put(1, 2);
                                 std::this_thread::sleep_for(d1);
                                 map.put(2, 3);
                                 std::this_thread::sleep_for(d2);
                                 map.put(3, 4);
                             });

    std::thread put_thread_2([&]()
                             {
                                 map.put(5, 6);
                                 std::this_thread::sleep_for(d1);
                                 map.put(7, 8);
                                 std::this_thread::sleep_for(d2);
                                 map.put(9, 10);
                             });

    put_thread_1.join();
    put_thread_2.join();

    auto get_thread_task = [&]()
    {
        EXPECT_EQ(map.get(1), 2);
        std::this_thread::sleep_for(d1);

        EXPECT_EQ(map.get(2), 3);
        std::this_thread::sleep_for(d2);

        EXPECT_EQ(map.get(3), 4);
        std::this_thread::sleep_for(d1);

        EXPECT_EQ(map.get(5), 6);
        std::this_thread::sleep_for(d2);

        EXPECT_EQ(map.get(7), 8);

        std::this_thread::sleep_for(d1);

        EXPECT_EQ(map.get(9), 10);
    };

    std::thread get_thread_1(get_thread_task);
    std::thread get_thread_2(get_thread_task);

    get_thread_1.join();
    get_thread_2.join();
}

TEST(MapTestSerial, Consistent)
{
    // arbitrary small number of buckets
    // to increase hash collisions.
    silicondb::map<int, int> map(3);
    test_map_consistency_serial(map);
}

TEST(MapTestSerial, UnmapTest)
{
    // arbitrary small number of buckets
    // to increase hash collisions.
    silicondb::map<int, int> map(3);
    test_map_unmap_serial(map);
}

TEST(MapTest, UnmapTest)
{
    // arbitrary small number of buckets
    // to increase hash collisions.
    silicondb::map<int, int> map(3);
    test_map_unmap(map);
}

TEST(MapTest, Constructs)
{
    silicondb::map<int, int> map;
}

TEST(MapTest, Consistent)
{
    // arbitrary small number of buckets
    // to increase hash collisions.
    silicondb::map<int, int> map(3);
    test_map_consistency(map);
}