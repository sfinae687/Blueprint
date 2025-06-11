//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/9.
//

module;

#include <functional>
#include <utility>

export module blueprint.dyn_node:host_api;
import :definition;

namespace blueprint::dyn_node::host_api
{
    template <typename T>
    using fn_t = std::move_only_function<T>;

    export struct host_api_t
    {
        fn_t<node_definition_proxy&(id_type) const> get_node_definition{};
        fn_t<type_definition_proxy&(id_type) const> get_type_definition{};
    };

    export const host_api_t *host;
}
