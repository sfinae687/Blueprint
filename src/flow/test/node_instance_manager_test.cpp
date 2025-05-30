//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-5.
//

#include <proxy.h>
#include <gtest/gtest.h>

#include <string>
#include <memory>

import blueprint.flow;
import blueprint.dyn_node;

using namespace blueprint::flow;
using namespace blueprint::dyn_node;

constexpr std::string_view TEST_NODE_ID = "test.node";

TEST(BlueprintFlowNode, NodeInstanceManage)
{
    using namespace blueprint::dyn_node;

    node_instance_manager mg{};

    node_definition_proxy p = std::make_unique<util::trivial_node_definition>(
        "Test", "Test Node", "test.node",
        input_sequence_t{}, output_sequence_t{}
    );
    auto test_instance = p->create_node();

    auto* pt1 = &proxy_cast<util::trivial_node_instance&>(*test_instance);

    const auto handler = mg.add_instance(std::move(test_instance));
    auto &&instance_in_handler = handler.node_instance();
    auto *pt2 = &proxy_cast<util::trivial_node_instance&>(*instance_in_handler);

    ASSERT_EQ(pt1, pt2);

    auto id = handler.node_id();
    auto handler2 = mg.get_handler(id);

    ASSERT_EQ(handler, handler2);
    ASSERT_NE(handler, nullptr);

    auto handler3 = mg.get_handler(0x114);
    ASSERT_EQ(handler3, nullptr);
    ASSERT_EQ(handler3, node_instance_handler{});

}