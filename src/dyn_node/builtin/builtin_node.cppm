//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//
module;
#include <proxy.h>

#include <memory>
#include <vector>
#include <optional>

export module blueprint.builtin_node;
import blueprint.dyn_node;
export import :signed_integral;
export import :unsigned_integral;

namespace blueprint::dyn_node::builtin
{

    // utility

    export node_definitions_t builtin_definitions()
    {
        node_definitions_t rt{};

        // auto idn = std::make_unique<identity_node_definition>();
        //
        // rt[IDENTITY_ID] = std::move(idn);
        return rt;
    }

    // utility

    template <typename T>
    auto definition_pair()
    {
        auto t = std::make_unique<T>();
        return std::pair(t->name(), std::move(t));
    }

    export type_definitions_t builtin_type_definitions()
    {
        type_definitions_t rt{};
        rt.insert(definition_pair<sint_definition>());
        return rt;
    }
}
