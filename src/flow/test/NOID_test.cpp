//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-5.
//

#include <gtest/gtest.h>

import blueprint.flow;

using namespace blueprint::flow;

constexpr no_id base_id = 0x12af;
constexpr no_id test_index = 2;
constexpr no_id test_output_id = 0x12afa;
constexpr no_id test_input_id = 0x12af2;

TEST(BlueprintFlowNoid, ChannelId)
{
    EXPECT_EQ(output_channel_id(base_id, test_index), test_output_id);
    EXPECT_EQ(input_channel_id(base_id, test_index), test_input_id);

    for (int i=0; i<8; ++i)
    {
        EXPECT_EQ(input_channel_id(0xabcd, i), 0xabcd0 + i);
        EXPECT_EQ(output_channel_id(0xabcd, i), 0xabcd8 + i);
    }
}

TEST(BlueprintFlowNoid, NodeId)
{
    EXPECT_EQ(node_id(test_output_id), base_id);
    EXPECT_EQ(node_id(test_input_id), base_id);
}

TEST(BlueprintFlowNoid, ChannelType)
{
    EXPECT_EQ(channel_type(test_output_id), channel_type_t::OUTPUT);
    EXPECT_EQ(channel_type(test_input_id), channel_type_t::INPUT);
}

TEST(BlueprintFlowNoid, ChannelIndex)
{
    EXPECT_EQ(channel_index(test_output_id), test_index);
    EXPECT_EQ(channel_index(test_input_id), test_index);
}

