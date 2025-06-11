//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//

module;

#include <proxy/proxy.h>

#include <imgui.h>
#include <Eigen/Dense>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/eigen.hpp>

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
            if (mat.rows() > 10 || mat.cols() > 10)
            {
                auto mat_dis = std::format("Matrix({}x{})", mat.rows(), mat.cols());
                ImGui::Text("%s", mat_dis.c_str());
            }
            else
            {
                GUI::matrix_editor(context.id, mat);
            }
        }
        else
        {
            ImGui::Text("Unknown Matrix");
        }
    }

    // Nodes //

    // matrix editor

    inline const char *items[] = {
        "Custom",
        "Laplace kernel",
        "Vertical sobel",
        "Horizontal sobel",
        "Vertical Prewitt",
        "Horizontal Prewitt",
        "Gaussian kernel",
        "Zeros",
        "Ones",
        "Eye",
    };

    export void draw_matrix_editor(node_draw_context &ctx)
    {
        using namespace blueprint::builtin;

        auto &&nd = ctx.node;
        auto node_id = ctx.id;

        auto &&editor = proxy_cast<matrix_editor_node&>(*nd);
        auto &&mat = *editor.mat_;
        auto &&cur_item = editor.cur_item;
        auto &&arg = editor.arg_;


        auto combo_lab = std::format("Mat type##{}", node_id);

        ImGui::SetNextItemWidth(128 + 32);
        bool combo_changed = ImGui::Combo(combo_lab.c_str(), &cur_item, items, IM_ARRAYSIZE(items));

        using enum matrix_editor_node::mat_type;

        switch (cur_item)
        {
        case Custom:
            if (GUI::matrix_editor(node_id, mat))
            {
                ctx.set_dirty = true;
            }
            break;
        case Laplace_kernel:
        case Vertical_sobel:
        case Horizontal_sobel:
        case Vertical_Prewitt:
        case Horizontal_Prewitt:
            if (combo_changed)
            {
                ctx.set_dirty = true;
            }
            break;
        case Gaussian_kernel:
        {
            if (combo_changed)
            {
                arg = matrix_editor_node::gaussian_kernel_context {
                    3, 1.0
                };
            }

            auto &&gua = std::get<matrix_editor_node::gaussian_kernel_context>(arg);
            bool changed = combo_changed;
            ImGui::SetNextItemWidth(128);
            if (ImGui::InputInt("ksize", &gua.sz))
            {
                changed  = true;
            }
            ImGui::SetNextItemWidth(128);
            if (ImGui::InputDouble("gamma", &gua.gamma))
            {
                changed = true;
            }
            if (gua.sz <= 0 || gua.sz % 2 != 1)
            {
                ImGui::Text("Unexpected Arguments");
            }
            else if (changed)
            {
                ctx.set_dirty = true;
            }
            break;
        }
        case Ones:
        case Zeros:
        {
            using ctx_t = matrix_editor_node::size_context;
            if (combo_changed)
            {
                arg = ctx_t {1, 1};
            }
            auto &&sz_ctx = std::get<ctx_t>(arg);
            bool changed = combo_changed;
            ImGui::SetNextItemWidth(128);
            if (ImGui::InputInt("Height", &sz_ctx.height))
            {
                changed = true;
            }
            ImGui::SetNextItemWidth(128);
            if (ImGui::InputInt("Width", &sz_ctx.width))
            {
                changed = true;
            }

            if (changed)
            {
                ctx.set_dirty = true;
            }
            break;
        }
        case Eye:
        {
            using ctx_t = matrix_editor_node::eye_context;
            if (combo_changed)
            {
                arg = ctx_t {1};
            }
            auto &&sz = std::get<ctx_t>(arg);
            bool changed = combo_changed;

            ImGui::SetNextItemWidth(128);
            if (ImGui::InputInt("Size", &sz.sz))
            {
                changed = true;
            }

            if (changed)
            {
                ctx.set_dirty = true;
            }
            break;
        }
        default:
            break;
        }

        if (ctx.set_dirty)
        {
            editor.flush_matrix();
        }
    }

}
