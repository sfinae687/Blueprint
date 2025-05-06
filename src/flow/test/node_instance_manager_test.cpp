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

struct test_node_instance
{
    [[nodiscard]] id_type type_id() const
    {
        return TEST_NODE_ID;
    }
    std::vector<id_type> channels() const {
        return {};
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
        return {};
    }
    data_proxy get_output(std::size_t)
    {
        return nullptr;
    }
};

static_assert(pro::proxiable<std::unique_ptr<test_node_instance>, node_instance_facade>);

struct test_node_definition
{
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
        return std::make_unique<test_node_instance>();
    }
};

static_assert(pro::proxiable<std::unique_ptr<test_node_definition>, node_definition_facade>);

TEST(BlueprintFlowNode, NodeInstanceManage)
{
    node_instance_manager mg{};

    node_definition_proxy p = std::make_unique<test_node_definition>();
    auto test_instance = p->create_node();

    auto* pt1 = &proxy_cast<test_node_instance&>(*test_instance);

    const auto handler = mg.add_instance(std::move(test_instance));
    auto &&instance_in_handler = handler.node_instance();
    auto *pt2 = &proxy_cast<test_node_instance&>(*instance_in_handler);

    ASSERT_EQ(pt1, pt2);

    auto id = handler.node_id();
    auto handler2 = mg.get_instance(id);

    ASSERT_EQ(handler, handler2);
    ASSERT_NE(handler, nullptr);

    auto handler3 = mg.get_instance(0x114);
    ASSERT_EQ(handler3, nullptr);
    ASSERT_EQ(handler3, node_instance_handler{});

}