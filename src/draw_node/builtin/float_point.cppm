//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/2.
//

module;

#include <proxy/proxy.h>

#include <imgui.h>

#include <utility>

export module blueprint.draw_node:float_point;
import blueprint.builtin_node;
import blueprint.dyn_node;
import :draw_rule;
import blueprint.gui;

namespace blueprint::draw_node
{
    constexpr std::size_t float_widget_max_size = 128;
    export void draw_float(data_draw_context &ctx)
    {
        auto &&data = ctx.data;
        if (ctx.is_connected)
        {
            if (data)
            {
                auto num = proxy_cast<builtin::builtin_float>(*data);
                ImGui::Text("%s", std::format("{}", num).c_str());
            }
            else
            {
                ImGui::Text("Unknown");
            }
        }
        else
        {
            bool reset = ! data.has_value();
            builtin::builtin_float num = data ? proxy_cast<builtin::builtin_float>(*data) : 0.;
            ImGui::Text("Float");
            ImGui::SameLine(0, 10.f);
            ImGui::SetNextItemWidth(float_widget_max_size);
            if (GUI::input_number("", num))
            {
                reset = true;
            }

            if (reset)
            {
                ctx.data = std::make_shared<builtin::builtin_float>(num);
            }
            ctx.set_data = reset;
        }
    }
}
