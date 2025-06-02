//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//
module;

#include <proxy/proxy.h>
#include <imgui.h>

export module blueprint.draw_node:integral;
import :draw_rule;
import blueprint.builtin_node;

namespace blueprint::draw_node
{
    using namespace builtin;
    using namespace dyn_node::builtin;

    constexpr std::size_t integral_widget_max_width = 128;

    // Integral

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
            ImGui::SetNextItemWidth(integral_widget_max_width);
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
            ImGui::SetNextItemWidth(integral_widget_max_width);
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

}
