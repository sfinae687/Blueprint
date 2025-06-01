//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//

module;

#include <proxy/proxy.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <boost/log/trivial.hpp>

#include <imgui.h>

#include <nfd.h>

export module blueprint.draw_node:image;
import blueprint.builtin_node;
import :draw_rule;
import :debug;

namespace blueprint::draw_node
{
    using namespace builtin;
    using namespace dyn_node::builtin;

    // Load

    export void draw_load_image(node_draw_context &ctx)
    {
        using dyn_node::util::constant_node_instance;
        auto &&nd = ctx.node;
        auto id = ctx.id;
        auto &&node_inst = proxy_cast<constant_node_instance&>(*nd);

        bool flush_flag = false;

        auto select_btn_id = std::format("Select File##{}", id);
        auto flush_btn_id = std::format("Flush##{}", id);
        if (ImGui::Button(select_btn_id.c_str()))
        {
             flush_flag = true;
        }
        // if (ImGui::Button(flush_btn_id.c_str()))
        // {
        //     flush_flag = true;
        // }
        if (flush_flag)
        {
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
                auto img = cv::imread(out_path);
                cv::cvtColor(img, img, cv::COLOR_BGR2RGBA);
                BOOST_LOG_SEV(draw_node_logger, info) << "Load image from: " << out_path;
                node_inst.set_output(std::make_shared<cv::Mat>(std::move(img)));
                ctx.set_dirty = true;
            }

            node_inst.set_context<std::string>(out_path);
        }

        if (node_inst.is_set())
        {
            ImGui::Text("Path:%s", node_inst.get_context<std::string&>().c_str());
        }

    }

    // Display

    export void draw_display_image(node_draw_context &ctx)
    {
        auto &&nd = proxy_cast<display_image_instance&>(*ctx.node);
        nd.flush_image();
        if (! nd.display())
        {
            ImGui::Text("Unable to display the image");
        }

    }

}
