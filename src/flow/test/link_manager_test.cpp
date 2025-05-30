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

namespace ranges = std::ranges;

using namespace blueprint::flow;
using namespace blueprint::dyn_node;
using util::trivial_node_definition;
using util::trivial_node_instance;

constexpr std::string_view TEST_TYPE2_ID = "test.int1";
constexpr std::string_view TEST_TYPE1_ID = "test.int2";
constexpr std::string_view TEST_NODE_ID = "test.node";

TEST(BlueprintFlowFlow, FlowTest)
{
    node_instance_manager mg{};
    link_manager lk(mg);

    node_definition_proxy t12t1 = std::make_unique<trivial_node_definition>(
        "t12t1", "test", "test.t12t1",
        input_sequence_t{TEST_TYPE1_ID}, output_sequence_t{TEST_TYPE1_ID}
    );
    auto t1t1_inst = t12t1->create_node();

    node_definition_proxy t22t2 = std::make_unique<trivial_node_definition>(
        "t22t2", "Test", "test.t22t2",
        input_sequence_t{TEST_TYPE2_ID}, output_sequence_t{TEST_TYPE2_ID}
    );
    auto t2t2_inst = t22t2->create_node();

    node_definition_proxy t12t2 = std::make_unique<trivial_node_definition>(
        "t12t2", "Test", "test.t12t2",
        input_sequence_t{TEST_TYPE1_ID}, output_sequence_t{TEST_TYPE2_ID}
    );
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
    ASSERT_TRUE(lk_id1) << "Unable to create a link";
    EXPECT_TRUE(lk.have_connection(id3ch1));
    EXPECT_TRUE(lk.is_connected(id1och1, id3ch1));
    EXPECT_FALSE(lk.is_connected(id1och1, id1ch1));

    auto id2ch1 = input_channel_id(id2, 0);
    auto lk_id1_rp = lk.create_link(id1och1, id3ch1);
    auto lk_id_empty = lk.create_link(id1och1, id2ch1);
    ASSERT_TRUE(lk_id1_rp) << "Failed to get the link id of existing link";
    EXPECT_FALSE(lk_id_empty) << "Create link with unmatched channel type";
    EXPECT_EQ(*lk_id1, *lk_id1_rp) << "The link id for a existing link is not same";

}

auto create_def(input_sequence_t input, output_sequence_t output)
{
    return std::make_unique<trivial_node_definition>(
        "Test", "test", "test.any_gen",
        std::move(input), std::move(output)
    );
}

TEST(BlueprintFlowFlow, QueryTest)
{
    node_instance_manager mg{};
    link_manager lk(mg);

    node_definition_proxy t1 = create_def({TEST_TYPE1_ID}, {});
    node_definition_proxy t2 = create_def({}, {TEST_TYPE2_ID});
    node_definition_proxy t1t1 = create_def({TEST_TYPE1_ID}, {TEST_TYPE1_ID});
    node_definition_proxy t2t2 = create_def({TEST_TYPE2_ID}, {TEST_TYPE2_ID});
    node_definition_proxy t1t2 = create_def({TEST_TYPE1_ID}, {TEST_TYPE2_ID});

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