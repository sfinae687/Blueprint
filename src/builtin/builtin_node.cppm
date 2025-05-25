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

namespace blueprint::dyn_node::builtin
{

    export plugin::component_package builtin_components()
    {
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

        plugin::component_package pkg{
            .types = {{std::move(signed_int), std::move(unsigned_int)}},
            .nodes = {{std::move(signed_node), std::move(unsigned_node)}},
            .groups = {
                {
                    "Core", {signed_node_id, unsigned_node_id}
                },
            },
        };

        return pkg;
    }

}
