#include <silicondb/map.h>
#include <gtest/gtest.h>

#include <thread>
#include <chrono>

template <typename integer_map>
void test_map_consistency_serial(integer_map &map)
{
    for (int i = 0; i < 10; i++)
    {
        map.put(i, i + 1);
    }

    for (int i = 0; i < 10; i++)
    {
        EXPECT_EQ(map.get(i), i + 1);
    }
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
    silicondb::map<int, int> map;
    test_map_consistency_serial(map);
}

TEST(MapTest, Constructs)
{
    silicondb::map<int, int> map;
}

TEST(MapTest, Consistent)
{
    silicondb::map<int, int> map;
    test_map_consistency(map);
}