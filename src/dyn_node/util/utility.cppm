//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-14.
//

/// @file
/// The dyn_node module provides some simple, dynamic basic utility for building node and type.
/// More utility with generic and mata-programing is available from stk_node module.

module;
#include <proxy/proxy.h>

#include <ranges>
#include <span>
#include <utility>
#include <vector>

export module blueprint.dyn_node:utility;
import :definition;
export import :trivial_node;
export import :constant_factory;

namespace blueprint::dyn_node::util
{

    // primitive

    export bool passable(const node_instance_proxy &from, std::size_t fi, const node_instance_proxy &to, std::size_t ti) noexcept;
    export bool passable(const data_proxy &from, const node_instance_proxy &to, std::size_t ti) noexcept;

    /**
     * Get the current signature of a node.
     * @return The current signature of the given node.
     */
    export const signature_t& current_signature(const node_instance_proxy &) noexcept;

    /**
     * The primitive to call the operation of a node. It will process the type check and some kind of transform.
     */
    export bool node_invoke(node_instance_proxy&, data_sequence) noexcept;

} // namespace blueprint::dyn_node::util
