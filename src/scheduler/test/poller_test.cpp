//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-1.
//

#include <gtest/gtest.h>

#include <boost/log/utility/setup.hpp>

#include <atomic>
#include <thread>
#include <iostream>

import blueprint.scheduler;

struct event_int
{
    using data_type = int;
};

struct event_double
{
    using data_type = double;
};

using namespace blueprint::scheduler;

TEST(BLUEPRINT_POLLER, EVENT_POLLER)
{
    event_queue<event_int, event_double> queue;
    event_poller pl(queue);

    std::atomic int_count = 0;
    std::atomic<double> double_count = 0;
    std::counting_semaphore event_finished(0);

    pl.set_callback([&] (double x)
    {
        double_count.fetch_add(x, std::memory_order::relaxed);
        event_finished.release();
    });

    pl.set_callback([&] (int x)
    {
        int_count.fetch_add(x, std::memory_order::relaxed);
        event_finished.release();
    });

    std::jthread polling_thread([&] (std::stop_token tk)
    {
        while (! tk.stop_requested())
        {
            pl.poll_once();
        }
    });

    queue.push(1);
    event_finished.acquire();
    ASSERT_EQ(int_count.load(std::memory_order::acquire), 1);
    queue.push(1);
    event_finished.acquire();
    ASSERT_EQ(int_count.load(std::memory_order::acquire), 2);
    queue.push(1.);
    event_finished.acquire();

    ASSERT_EQ(int_count.load(std::memory_order::acquire), 2);
    ASSERT_FLOAT_EQ(double_count.load(std::memory_order::acquire), 1.);


}