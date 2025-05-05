//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-30.
//

#include <boost/proto/transform/env.hpp>
#include <gtest/gtest.h>

import blueprint.scheduler;
using namespace blueprint::scheduler;

struct event1
{
    struct data_type
    {
        int i;
    };
};

struct event2
{
    using data_type = double;
};

TEST(BlueprintEvent, Queue)
{
    using std::abs;

    static_assert(event<event1>);
    static_assert(event<event2>);

    using e1_data = event_data_t<event1>;
    using e2_data = event_data_t<event2>;
    static_assert(std::same_as<e1_data, event1::data_type>);
    static_assert(std::same_as<e2_data, double>);

    event_queue<event1, event2> queue{};

    queue.push<event1>({1});
    queue.push<event2>(1);
    queue.push<event2>(1.);

    auto e1_pop_data = queue.pop<event1>();
    ASSERT_TRUE(e1_pop_data && e1_pop_data->i == 1);
    auto e2_pop_data = queue.pop<event2>();
    ASSERT_TRUE(e2_pop_data && *e2_pop_data - 1 <= std::numeric_limits<double>::epsilon());
    e2_pop_data = queue.pop<event2>();
    ASSERT_TRUE(e2_pop_data && *e2_pop_data - 1 <= std::numeric_limits<double>::epsilon());
    e2_pop_data = queue.pop<event2>();
    ASSERT_FALSE(e2_pop_data);

    queue.push(e1_data{10});
    e1_pop_data = queue.pop<event1>();
    ASSERT_TRUE(e1_pop_data && e1_pop_data->i == 10);
    e1_pop_data = queue.pop<event1>();
    ASSERT_FALSE(e1_pop_data);

    queue.push(1.1);
    e2_pop_data = queue.pop<event2>();
    ASSERT_TRUE(e2_pop_data && abs(*e2_pop_data - 1.1) <= std::numeric_limits<double>::epsilon());

}

TEST(BlueprintEvent, QueueConsume)
{
    event_queue<event1, event2> queue{};

    using data = event1::data_type;

    queue.push(data{12});
    queue.push(data{1});
    queue.push(3.14);
    queue.push(3.14);

    queue.consume_one<event1>([] (data e)
    {
         ASSERT_EQ(12, e.i);
    });
    queue.consume_one([] (data e)
    {
         ASSERT_EQ(1, e.i);
    });

}