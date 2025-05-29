//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-29.
//

#include <gtest/gtest.h>

#include <concepts>
#include <proxy.h>
#include <utility>

import blueprint.dyn_node;
import blueprint.stk_node;

using namespace blueprint::stk_node;

constexpr char int_id[] = "core.int";
struct int_td_t : basic_type_desc<int_id, int, void>
{
    template <std::signed_integral I>
    I transform(int x) const
    {
        return x;
    }
    template <std::signed_integral I>
    int accept(I x) const
    {
        return x;
    }
};
constexpr int_td_t int_td{};

constexpr char uint_id[] = "core.uint";
struct uint_td_t : basic_type_desc<uint_id, unsigned, void>
{
    template <std::unsigned_integral I>
    I transform(unsigned x) const
    {
        return x;
    }
    template <std::unsigned_integral I>
    unsigned accept(I x) const
    {
        return x;
    }
};
constexpr uint_td_t uint_td{};

TEST(TypeMapperTest, Mapper)
{
    mapper<int_td, uint_td> mp;
    auto t1 = mp.deduced_by_target<long>();
    auto t2 = mp.deduced_by_source<unsigned long>();
    static_assert(std::same_as<decltype(t1), int_td_t>);
    static_assert(std::same_as<decltype(t2), uint_td_t>);
}

