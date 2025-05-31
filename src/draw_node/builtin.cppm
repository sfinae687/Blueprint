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
import blueprint.gui;

namespace blueprint::draw_node
{
    using dyn_node::data_proxy;
    using namespace dyn_node;
    using namespace dyn_node::builtin;
    using namespace blueprint::builtin;

    constexpr std::size_t widget_max_width = 128;

    // Types //

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

    export void draw_matrix(data_draw_context &context)
    {
        if (context.data)
        {
            auto &&mat = proxy_cast<const builtin_matrix_t&>(*context.data);
            GUI::matrix_editor(context.id, mat);
        }
        else
        {
            ImGui::Text("Unknown Matrix");
        }
    }

    // Nodes //

    export void node_draw_noop(node_draw_context &)
    {
        /* NOOP */
    }

    export void draw_matrix_editor(node_draw_context &ctx)
    {
        using namespace blueprint::builtin;

        auto &&nd = ctx.node;
        auto node_id = ctx.id;

        auto &&editor = proxy_cast<matrix_editor_node&>(*nd);
        auto &&mat = *editor.mat_;
        if (GUI::matrix_editor(node_id, mat))
        {
            ctx.set_dirty = true;
        }
    }

}
