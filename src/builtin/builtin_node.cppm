//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//
module;
#include <proxy.h>

#include <memory>
#include <vector>
#include <string>
#include <string_view>

export module blueprint.builtin_node;
import blueprint.dyn_node;
import blueprint.plugin;
export import :signed_integral;
export import :unsigned_integral;
export import :identity_node;
export import :integral_arithmetic;

namespace blueprint::dyn_node::builtin
{

    export plugin::component_package builtin_components()
    {
        using namespace blueprint::builtin;

        using std::make_shared;
        using std::make_unique;

        type_definition_proxy signed_int = make_shared<sint_definition>();
        type_definition_proxy unsigned_int = make_shared<uint_definition>();
        auto signed_id = signed_int->id();
        auto unsigned_id = unsigned_int->id();

        node_definition_proxy signed_node = make_shared<identity_node_definition>(signed_int);
        node_definition_proxy unsigned_node = make_shared<identity_node_definition>(unsigned_int);
        auto signed_node_id = signed_node->id();
        auto unsigned_node_id = unsigned_node->id();

        node_definition_proxy signed_add = make_shared<sint_plus_node>();
        node_definition_proxy unsigned_add = make_shared<uint_plus_node>();
        node_definition_proxy signed_sub = make_shared<sint_sub_node>();
        node_definition_proxy  unsigned_sub = make_shared<uint_sub_node>();
        node_definition_proxy signed_mul = make_shared<sint_mul_node>();
        node_definition_proxy unsigned_mul = make_shared<uint_mul_node>();
        auto signed_plus_id = signed_add->id();
        auto unsigned_plus_id = unsigned_add->id();
        auto signed_sub_id = signed_sub->id();
        auto unsigned_sub_id = unsigned_sub->id();
        auto signed_mul_id = signed_mul->id();
        auto unsigned_mul_id = unsigned_mul->id();

        plugin::component_package pkg{
            .types = {{std::move(signed_int), std::move(unsigned_int)}},
            .nodes = {
                {
                    std::move(signed_node),
                    std::move(unsigned_node),
                    std::move(signed_add),
                    std::move(unsigned_add),
                    std::move(signed_sub),
                    std::move(unsigned_sub),
                    std::move(signed_mul),
                    std::move(unsigned_mul)
                }
            },
            .groups = {
                {
                    "Core", {signed_node_id, unsigned_node_id}
                },
                {
                    "Arithmetic", {
                        signed_plus_id, unsigned_plus_id,
                        signed_sub_id, unsigned_sub_id,
                        signed_mul_id, unsigned_mul_id
                    }
                },
            },
        };

        return pkg;
    }

}
