//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/8.
//

module;

#include <functional>

export module blueprint.dyn_node:host_api;
import :definition;

namespace blueprint::dyn_node::host_api
{
    export template <typename T>
    using fn_t = std::move_only_function<T>;

    export struct api_t
    {
        fn_t<type_definition_proxy& (id_type)> get_type_definition{};
        fn_t<node_definition_proxy& (id_type)> get_node_definition{};
    };

    export inline api_t *host = nullptr;

}
