#include <silicondb/map.h>
#include <gtest/gtest.h>

#include <thread>
#include <chrono>

TEST(MapTest, Constructs)
{
    silicondb::map<int, int> map;
}

TEST(MapTest, Consistent)
{
    using namespace std::chrono_literals;
    silicondb::map<int, int> map;

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

    std::thread get_thread_1([&]()
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
                             });

    std::thread get_thread_2([&]()
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
                             });

    get_thread_1.join();
    get_thread_2.join();
}