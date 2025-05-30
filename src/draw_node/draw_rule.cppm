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

    export enum class data_channel_type_t
    {
        input,
        output,
    };

    export struct data_draw_context
    {
        std::string_view id; ///< [in]
        data_channel_type_t channel; ///< [in], the input or output type of the data
        bool is_connected; ///< [in], the connection state of given data.

        /// [in,out], the data bind to the channel. If it's not available, it is set to nullptr
        dyn_node::data_proxy data;

        /// [out], reset the data to the given channel. It only used when `is_connected` is false.
        bool set_data;
    };

    export using type_draw_operation = std::move_only_function<void(data_draw_context&)>;
    export using type_draw_map_t = std::unordered_map<dyn_node::id_type, type_draw_operation>;




    export using node_additional_draw_operation = std::move_only_function<void(const dyn_node::node_instance_proxy &)>;
    export using node_draw_map_t = std::unordered_map<dyn_node::id_type, node_additional_draw_operation>;
}
