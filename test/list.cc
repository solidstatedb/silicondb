#include <silicondb/list.h>
#include <gtest/gtest.h>

#include <thread>
#include <chrono>

#include <iostream>

TEST(ListTestSerial, Constructs)
{
    silicondb::list<int> list;
}

TEST(ListTestSerial, PushFront)
{
    silicondb::list<int> list;

    for (int i{0}; i < 10; ++i)
        list.push_front(i);

    int n = 9;

    list.for_each([&](int n_)
                  { EXPECT_EQ(n_, n--); });
}

TEST(ListTestSerial, RemoveIf)
{
    silicondb::list<int> list;

    for (int i{0}; i < 10; i++)
        list.push_front(i);

    list.remove_if([](int n)
                   { return (n & 1) == 0; });

    int n = 9;

    list.for_each([&](int n_)
                  {
                      EXPECT_EQ(n_, n);
                      n -= 2;
                  });
}

TEST(ListTestSerial, FindFirstIf)
{
    silicondb::list<int> list;

    list.push_front(592);
    list.push_front(763);

    std::shared_ptr<int> result;

    result = list.find_first_if([](int n) -> bool
                                { return n == 763; });

    ASSERT_EQ(bool(result), true);
    EXPECT_EQ(*result, 763);

    result = list.find_first_if([](int n) -> bool
                                { return n < 10; });

    EXPECT_EQ(bool(result), false);

    list.remove_if([](int n) -> bool
                   { return n < 600 && n > 500; });

    result = list.find_first_if([](int n) -> bool
                                { return n < 600 && n > 500; });
    EXPECT_EQ(bool(result), false);
}

TEST(ListTest, PushFront)
{
    using namespace std::chrono_literals;
    auto d1 = 57ms, d2 = 29ms; // random wait durations

    silicondb::list<int> list;

    std::thread push_thread_1([&]()
                              {
                                  for (int i{1}; i <= 10; i += 2)
                                  {
                                      list.push_front(i);
                                      std::this_thread::sleep_for(d1);
                                  }
                              });

    std::thread push_thread_2([&]()
                              {
                                  for (int i{10}; i >= 1; i -= 2)
                                  {
                                      list.push_front(i);
                                      std::this_thread::sleep_for(d2);
                                  }
                              });

    push_thread_1.join();
    push_thread_2.join();

    bool present[10];

    for (int i = 0; i < 10; i++)
        present[i] = false;

    list.for_each([&](int n)
                  { present[n - 1] = true; });

    for (int i = 0; i < 10; i++)
        EXPECT_EQ(present[i], true);
}

TEST(ListTest, RemoveIf)
{
    using namespace std::chrono_literals;
    auto d1 = 57ms, d2 = 29ms; // random wait durations

    silicondb::list<int> list;

    for (int i{1}; i <= 10; i++)
        list.push_front(i);

    std::thread remover_thread_1(
        [&]()
        {
            list.remove_if(
                [&](int n)
                {
                    std::this_thread::sleep_for(d1);
                    return n % 2 == 0;
                });
        });

    std::thread remover_thread_2(
        [&]()
        {
            list.remove_if(
                [&](int n)
                {
                    std::this_thread::sleep_for(d2);
                    return n % 3 == 0;
                });
        });

    remover_thread_1.join();
    remover_thread_2.join();

    bool present[10];

    for (int i = 0; i < 10; i++)
        present[i] = false;

    std::thread reader_thread(
        [&]()
        {
            list.for_each(
                [&](int n)
                { present[n - 1] = true; });
        });

    reader_thread.join();

    for (int i = 0; i < 10; i++)
    {
        int n = i + 1;

        if (n % 2 == 0 || n % 3 == 0)
            EXPECT_EQ(present[i], false);
        else
            EXPECT_EQ(present[i], true);
    }
}
