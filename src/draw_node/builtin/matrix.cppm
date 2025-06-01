//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//

module;

#include <proxy/proxy.h>

#include <imgui.h>

export module blueprint.draw_node:matrix;
import blueprint.builtin_node;
import blueprint.gui;
import :draw_rule;

namespace blueprint::draw_node
{
    using namespace builtin;
    using namespace dyn_node::builtin;

    // Types //

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

    // matrix editor

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
