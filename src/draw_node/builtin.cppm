//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//

module;
#include <proxy.h>
#include <imgui.h>

#include <memory>
#include <format>

export module blueprint.draw_node:builtin;
import :draw_rule;
import blueprint.dyn_node;
import blueprint.builtin_node;

namespace blueprint::draw_node
{
    using dyn_node::data_proxy;
    using namespace dyn_node;
    using namespace dyn_node::builtin;

    constexpr std::size_t widget_max_width = 128;

    // Types

    export void draw_signed(data_draw_context &context)
    {

        if (context.is_connected)
        {
            if (context.data)
            {
                auto num = proxy_cast<builtin_signed_type>(*context.data);
                ImGui::Text("%s", std::format("{}", num).c_str());
            }
            else
            {
                ImGui::Text("Unknown");
            }
        }
        else
        {
            bool reset = false;
            builtin_signed_type num = 0;
            if (!context.data)
            {
                reset = true;
            }
            else
            {
                num = proxy_cast<builtin_signed_type>(*context.data);
            }
            ImGui::Text("Integral");
            ImGui::SameLine(0, 10.f);
            ImGui::SetNextItemWidth(widget_max_width);
            if (ImGui::InputScalar("",  ImGuiDataType_S64, &num))
            {
                reset = true;
            }

            if (reset)
            {
                context.data = std::make_shared<builtin_signed_type>(num);
            }
            context.set_data = reset;
        }
    }

    export void draw_unsigned(data_draw_context &context)
    {
        ImGui::SetNextItemWidth(widget_max_width);
        if (context.is_connected)
        {
            if (context.data)
            {
                auto num = proxy_cast<builtin_unsigned_type>(*context.data);
                ImGui::Text("%s", std::format("{}", num).c_str());
            }
            else
            {
                ImGui::Text("Unknown");
            }
        }
        else
        {
            bool reset = false;
            builtin_unsigned_type num = 0;
            if (!context.data)
            {
                reset = true;
            }
            else
            {
                num = proxy_cast<builtin_unsigned_type>(*context.data);
            }

            ImGui::Text("Unsigned");
            ImGui::SameLine(0, 10.f);
            ImGui::SetNextItemWidth(widget_max_width);
            if (ImGui::InputScalar("", ImGuiDataType_U64, &num))
            {
                reset = true;
            }

            if (reset)
            {
                context.data = std::make_shared<builtin_unsigned_type>(num);
            }
            context.set_data = reset;
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
        rt[UNSIGNED_INTEGRAL_ID] = &draw_unsigned;

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
