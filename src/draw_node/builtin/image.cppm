//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//

module;

#include <proxy/proxy.h>
#include <opencv2/core.hpp>

#include <boost/log/trivial.hpp>

#include <imgui.h>

#include <nfd.h>

export module blueprint.draw_node:image;
import blueprint.builtin_node;
import blueprint.gui;
import :draw_rule;
import :debug;

namespace blueprint::draw_node
{
    using namespace builtin;
    using namespace dyn_node::builtin;

    // Type

    export void draw_image_info(data_draw_context &ctx)
    {
        auto &&d = ctx.data;
        if (ctx.is_connected && ctx.data)
        {
            auto &&img = proxy_cast<builtin_image_t&>(*d);

            auto info = std::format("Image({}x{}, {} channel(s))", img.rows, img.cols, img.channels());
            ImGui::Text("%s", info.c_str());
        }
        else
        {
            ImGui::Text("Image Unspecified");
        }
    }

    // Load

    export void draw_load_image(node_draw_context &ctx)
    {
        using dyn_node::util::constant_node_instance;
        auto &&nd = ctx.node;
        auto id = ctx.id;
        auto &&node_inst = proxy_cast<load_image_instance&>(*nd);

        bool flush_flag = false;

        auto select_btn_id = std::format("Select File##{}", id);
        auto flush_btn_id = std::format("Flush##{}", id);
        if (ImGui::Button(select_btn_id.c_str()))
        {
            flush_flag = true;
            nfdu8char_t *out_path;
            nfdu8filteritem_t filters[] = {
            {"Picture", "jpg,jpeg,png"}
            };
            nfdopendialogu8args_t args = {0};
            args.filterList = filters;
            args.filterCount = std::size(filters);

            nfdresult_t dialog_result = NFD_OpenDialogU8_With(&out_path, &args);
            if (dialog_result == NFD_OKAY)
            {
                node_inst.set_path(out_path);
                BOOST_LOG_SEV(draw_node_logger, info) << "Load image from: " << out_path;
                ctx.set_dirty = true;
            }

        }
        if (node_inst.has_image())
        {
            ImGui::Text("Path:%s", node_inst.path().c_str());
        }

    }

    // Display

    export void draw_display_image(node_draw_context &ctx)
    {
        auto &&nd = proxy_cast<display_image_instance&>(*ctx.node);
        if (nd.has_image())
        {
            auto id = ctx.id;

            float width = nd.width() * nd.scale;

            bool config_changed = false;

            ImGui::SetNextItemWidth(width);
            auto slider_id = std::format("scale##{}", id);
            config_changed |= ImGui::SliderFloat(slider_id.c_str(), &nd.scale, 0.1, 1.0);

            ImGui::SetNextItemWidth(width);
            auto normalize_id = std::format("normalize##{}", id);
            config_changed |= ImGui::Checkbox(normalize_id.c_str(), &nd.normalize);


            ImGui::SetNextItemWidth(width);
            auto alpha_id = std::format("alpha##{}", id);
            config_changed |= GUI::input_number(alpha_id, nd.alpha);

            ImGui::SetNextItemWidth(width);
            auto beta_id = std::format("beta##{}", id);
            config_changed |= GUI::input_number(beta_id, nd.beta);

            if (config_changed)
            {
                nd.set_flush();
            }
            nd.flush_image();

            ImGui::SetNextItemWidth(width);

            nd.display();
        }
        else
        {
            ImGui::Text("Unable to display the image");
        }

    }

}
