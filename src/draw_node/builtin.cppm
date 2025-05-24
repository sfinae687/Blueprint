//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//

module;
#include <proxy.h>
#include <imgui.h>

#include <memory>

export module blueprint.draw_node:builtin;
import :draw_rule;
import blueprint.dyn_node;
import blueprint.builtin_node;

namespace blueprint::draw_node
{
    using dyn_node::data_proxy;
    using namespace dyn_node;

    // Types

    export void draw_signed(data_proxy &d, data_draw_context context)
    {
        if (! context.is_connected && context.channel == data_channel_type_t::input)
        {
            using std::make_unique;
            if (!d)
            {
                d = std::make_shared<builtin::builtin_signed_type>(0);
            }

            int &data_d = proxy_cast<int&>(*d);
            ImGui::InputInt("Int", &data_d);
        }
    }

    // Nodes

    export void node_draw_noop(dyn_node::node_instance_proxy)
    {
        /* NOOP */
    }

    // utility

    export type_draw_map_t builtin_type_draw_map()
    {
        using namespace dyn_node;
        using namespace dyn_node::builtin;

        type_draw_map_t rt;
        rt[SIGNED_INTEGRAL_ID] = &draw_signed;

        return rt;
    }

    export node_draw_map_t builtin_node_draw_map()
    {
        using namespace dyn_node;
        using namespace dyn_node::builtin;

        node_draw_map_t rt;
        // rt[IDENTITY_ID] = &node_draw_noop;

        return rt;
    }
}
