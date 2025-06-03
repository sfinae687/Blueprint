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
        auto &&cur_item = editor.cur_item;
        auto &&arg = editor.arg_;

        const char *items[] = {
            "Custom",
            "Laplace kernel",
            "Vertical sobel",
            "Horizontal sobel",
            "Vertical Prewitt",
            "Horizontal Prewitt",
            "Gaussian kernel",
        };

        auto combo_lab = std::format("Mat type##{}", node_id);

        ImGui::SetNextItemWidth(128 + 32);
        bool combo_changed = ImGui::Combo(combo_lab.c_str(), &cur_item, items, IM_ARRAYSIZE(items));

        switch (cur_item)
        {
        case 0:
            if (GUI::matrix_editor(node_id, mat))
            {
                ctx.set_dirty = true;
            }
            break;
        case 1:
            if (combo_changed)
            {
                mat = builtin_matrix_t{
                    {0, 1, 0},
                    {1, -4, 1},
                    {0, 1, 0},
                };
                ctx.set_dirty = true;
            }
            break;
        case 2:
            if (combo_changed)
            {
                mat = builtin_matrix_t {
                    {1, 2, 1},
                    {0, 0, 0},
                    {-1, -2, -1},
                };
                ctx.set_dirty= true;
            }
            break;
        case 3:
            if (combo_changed)
            {
                mat = builtin_matrix_t {
                    {1, 0, -1},
                    {2, 0, -2},
                    {1, 0, -1},
                };
                ctx.set_dirty= true;
            }
            break;
        case 4:
            if (combo_changed)
            {
                mat = builtin_matrix_t{
                    {1, 1, 1},
                    {0, 0, 0},
                    {-1, -1, -1},
                };
                ctx.set_dirty = true;
            }
        case 5:
            if (combo_changed)
            {
                mat = builtin_matrix_t{
                    {1, 0, -1},
                    {1, 0, -1},
                    {1, 0, -1},
                };
                ctx.set_dirty = true;
            }
        case 6:
        {
            if (combo_changed)
            {
                arg = matrix_editor_node::gaussian_kernel_context {
                    3, 1.0
                };
            }

            auto &&gua = std::get<matrix_editor_node::gaussian_kernel_context>(arg);
            bool changed = false;
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
                auto kernel = cv::getGaussianKernel(gua.sz, gua.gamma, CV_64F);
                kernel = kernel * kernel.t();
                cv:cv2eigen(kernel, mat);
                ctx.set_dirty = true;
            }
        }
        default:
            break;
        }
    }

}
