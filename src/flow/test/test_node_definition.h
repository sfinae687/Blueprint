//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-6.
//

#ifndef TEST_NODE_DEFINITION_H
#define TEST_NODE_DEFINITION_H

#include <proxy.h>

#include <string_view>
#include <vector>

import blueprint.dyn_node;

using namespace blueprint::dyn_node;

constexpr std::string_view TEST_TYPE_ID = "test.type1";
constexpr std::string_view TEST_NODE_ID = "test.node";

struct test_node_instance
{
    [[nodiscard]] id_type type_id() const
    {
        return TEST_NODE_ID;
    }
    std::vector<id_type> channels() const {
        return {TEST_TYPE_ID};
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
        return {TEST_TYPE_ID};
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

#endif //TEST_NODE_DEFINITION_H
