//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//

module;
#include <proxy.h>

#include <functional>
#include <unordered_map>

export module blueprint.draw_node:draw_rule;
import blueprint.dyn_node;

namespace blueprint::draw_node
{

    enum class data_channel_type_t
    {
        input,
        output,
    };

    struct data_draw_context
    {
        data_channel_type_t channel;
        bool is_connected;
    };

    using type_draw_operation = std::move_only_function<void(dyn_node::data_proxy &, data_draw_context)>;
    using type_draw_map_t = std::unordered_map<dyn_node::id_type, type_draw_operation>;

    using node_additional_draw_operation = std::move_only_function<void(dyn_node::node_instance_proxy)>;
    using node_draw_map_t = std::unordered_map<dyn_node::id_type, node_additional_draw_operation>;
}
