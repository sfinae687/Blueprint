//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-5.
//

#include <proxy.h>
#include <gtest/gtest.h>

#include <ranges>
#include <algorithm>

import blueprint.flow;
import blueprint.dyn_node;

using namespace blueprint::flow;
using namespace blueprint::dyn_node;
namespace ranges = std::ranges;

constexpr std::string_view TEST_TYPE2_ID = "test.int1";
constexpr std::string_view TEST_TYPE1_ID = "test.int2";
constexpr std::string_view TEST_NODE_ID = "test.node";

struct test_node_instance
{
    test_node_instance(std::string_view from, std::string_view to)
        :from(from)
        ,to(to)
    {

    }

    [[nodiscard]] id_type type_id() const
    {
        return TEST_NODE_ID;
    }
    std::vector<id_type> channels() const {
        if (from.empty())
        {
            return {};
        }
        return {from};
    }
    [[nodiscard]] bool set_channel(std::size_t, data_proxy) const
    {
        return false;
    }
    data_proxy get_channel(std::size_t)
    {
        return nullptr;
    }
    void compute() const
    {
    }
    std::vector<id_type> outputs() const
    {
        if (from.empty())
        {
            return {};
        }
        return {to};
    }
    data_proxy get_output(std::size_t)
    {
        return nullptr;
    }

    std::string_view from;
    std::string_view to;
};

static_assert(pro::proxiable<std::unique_ptr<test_node_instance>, node_instance_facade>);

struct test_node_definition
{
    test_node_definition(std::string_view from, std::string_view to)
        : from(from)
        , to(to)
    {

    }

    text_type name() const {
        return "Test";
    }
    text_type description() const
    {
        return "Test";
    }
    id_type id() const
    {
        return TEST_NODE_ID;
    }
    node_instance_proxy create_node()
    {
        return std::make_unique<test_node_instance>(from, to);
    }

    std::string_view from;
    std::string_view to;
};

static_assert(pro::proxiable<std::unique_ptr<test_node_definition>, node_definition_facade>);

TEST(BlueprintFlowFlow, FlowTest)
{
    node_instance_manager mg{};
    link_manager lk(mg);

    node_definition_proxy t12t1 = std::make_unique<test_node_definition>(TEST_TYPE1_ID, TEST_TYPE1_ID);
    auto t1t1_inst = t12t1->create_node();

    node_definition_proxy t22t2 = std::make_unique<test_node_definition>(TEST_TYPE2_ID, TEST_TYPE2_ID);
    auto t2t2_inst = t22t2->create_node();

    node_definition_proxy t12t2 = std::make_unique<test_node_definition>(TEST_TYPE1_ID, TEST_TYPE2_ID);
    auto t1t2_inst = t12t2->create_node();

    auto h1 = mg.add_instance(std::move(t1t1_inst));
    auto h2 = mg.add_instance(std::move(t2t2_inst));
    auto h3 = mg.add_instance(std::move(t1t2_inst));

    auto id1 = h1.node_id();
    auto id2 = h2.node_id();
    auto id3 = h3.node_id();

    auto id1ch1 = input_channel_id(id1, 0);
    auto id1och1 = output_channel_id(id1, 0);

    EXPECT_FALSE(lk.is_connected(id1och1, id1ch1));
    EXPECT_FALSE(lk.have_connection(id1ch1));

    auto id3ch1 = input_channel_id(id3, 0);

    EXPECT_FALSE(lk.have_connection(id3ch1));

    auto lk_id1 = lk.create_link(id1och1, id3ch1);
    ASSERT_TRUE(lk_id1);
    EXPECT_TRUE(lk.have_connection(id3ch1));
    EXPECT_TRUE(lk.is_connected(id1och1, id3ch1));
    EXPECT_FALSE(lk.is_connected(id1och1, id1ch1));

    auto id2ch1 = input_channel_id(id2, 0);
    auto lk_id1_rp = lk.create_link(id1och1, id3ch1);
    auto lk_id_empty = lk.create_link(id1och1, id2ch1);
    ASSERT_TRUE(lk_id1_rp);
    EXPECT_FALSE(lk_id_empty);
    EXPECT_EQ(*lk_id1, *lk_id1_rp);

}

TEST(BlueprintFlowFlow, QueryTest)
{
    node_instance_manager mg{};
    link_manager lk(mg);

    node_definition_proxy t1 = std::make_unique<test_node_definition>(TEST_TYPE1_ID, "");
    node_definition_proxy t2 = std::make_unique<test_node_definition>("", TEST_TYPE2_ID);
    node_definition_proxy t1t1 = std::make_unique<test_node_definition>(TEST_TYPE1_ID, TEST_TYPE1_ID);
    node_definition_proxy t2t2 = std::make_unique<test_node_definition>(TEST_TYPE2_ID, TEST_TYPE2_ID);
    node_definition_proxy t1t2 = std::make_unique<test_node_definition>(TEST_TYPE1_ID, TEST_TYPE2_ID);

    auto t1_inst = t1->create_node();
    auto t2_inst = t2->create_node();
    auto t1t1_inst = t1t1->create_node();
    auto t2t2_inst = t2t2->create_node();
    auto t1t2_inst = t1t2->create_node();

    auto t1h = mg.add_instance(std::move(t1_inst));
    auto t2h = mg.add_instance(std::move(t2_inst));
    auto t1t1h = mg.add_instance(std::move(t1t1_inst));
    auto t2t2h = mg.add_instance(std::move(t2t2_inst));
    auto t1t2h = mg.add_instance(std::move(t1t2_inst));

    auto t1_id = t1h.node_id();
    auto t2_id = t2h.node_id();
    auto t1t1_id = t1t1h.node_id();
    auto t2t2_id = t2t2h.node_id();
    auto t1t2_id = t1t2h.node_id();

    EXPECT_TRUE(lk.empty());

    // link query

    auto t1t1_i0 = input_channel_id(t1t1_id, 0);
    auto t1t1_o0 = output_channel_id(t1t1_id, 0);
    auto lk1 = lk.create_link(t1t1_o0, t1t1_i0);
    ASSERT_TRUE(lk1);

    auto t2t2_i0 = input_channel_id(t2t2_id, 0);
    auto t2t2_o0 = output_channel_id(t2t2_id, 0);
    auto lk2 = lk.create_link(t2t2_o0, t2t2_i0);
    ASSERT_TRUE(lk2);

    EXPECT_FALSE(lk.empty());

    auto io1 = lk.query_link(*lk1);
    ASSERT_TRUE(io1);
    EXPECT_EQ(io1->first ,t1t1_i0);
    EXPECT_EQ(io1->second, t1t1_o0);

    auto io2 = lk.query_link(*lk2);
    ASSERT_TRUE(io2);
    EXPECT_EQ(io2->first, t2t2_i0);
    EXPECT_EQ(io2->second, t2t2_o0);

    lk.remove_link(io2->second, io2->first);
    io2 = lk.query_link(*lk2);
    EXPECT_FALSE(io2);

    // input to output

    auto ot1 = lk.to_output(t1t1_i0);
    ASSERT_TRUE(ot1);
    EXPECT_EQ(*ot1, t1t1_o0);

    auto ot2 = lk.to_output(t2t2_i0);
    EXPECT_FALSE(ot2);

    // output to input

    auto it2 = lk.to_input(t2t2_o0);
    EXPECT_TRUE(it2.empty());

    auto t1_i0 = input_channel_id(t1_id, 0);
    auto lk3 = lk.create_link(t1t1_o0, t1_i0);
    ASSERT_TRUE(lk3);

    auto it1 = lk.to_input(t1t1_o0);
    ASSERT_EQ(it1.size(), 2);
    std::vector ans1{t1t1_i0, t1_i0};
    ranges::sort(ans1);
    ranges::sort(it1);
    EXPECT_EQ(ans1, it1);

}