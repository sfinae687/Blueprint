//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-26.
//


module;
#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imnodes.h>
#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <proxy.h>

module blueprint;
import blueprint.gui;
import blueprint.dyn_node;
import blueprint.draw_node;

namespace blueprint
{

    /// Constructor, it defines the all GUI, and connect another necessary component.
    blueprint_application::blueprint_application()
        : gui_("Blueprint Node editor", 720, 360)
        , nodes_(gui_)
    {
        using namespace GUI;

        setup_logger();
        load_builtin();

        gui_.set_draw_operation([&]
        {
            if (begin_main_window("main", &main_open))
            {

                ImGui::BeginChild("NodeEditor",
                    ImGui::GetContentRegionAvail() - ImVec2(0, ImGui::GetTextLineHeight() * 1.3F));

                ImNodes::BeginNodeEditor();
                ImNodes::BeginNode(0);
                ImGui::Dummy(ImVec2(80.0f, 45.0f));
                ImNodes::EndNode();
                ImNodes::EndNodeEditor();

                ImGui::EndChild();

                ImGui::Text("hello world");
            }
            ImGui::End();
        });
    }

    int blueprint_application::run()
    {
        gui_.render_loop();
        return 0;
    }

    void blueprint_application::setup_logger()
    {
        using namespace boost::log;
        logger.add_attribute("Module", attributes::constant<std::string>("Application"));
    }

    void blueprint_application::load_builtin()
    {
        BOOST_LOG_SEV(logger, info) << "Loading builtin Nodes";
        node_def_ = dyn_node::builtin::builtin_definitions();

        BOOST_LOG_SEV(logger, info) << "Loading builtin Types";
        type_def_ = dyn_node::builtin::builtin_type_definitions();

        BOOST_LOG_SEV(logger, info) << "Loading builtin Nodes draw rule";
        node_draw_ = draw_node::builtin_node_draw_map();

        BOOST_LOG_SEV(logger, info) << "Loading builtin Types draw rule";
        type_draw_ = draw_node::builtin_type_draw_map();

        BOOST_LOG_SEV(logger, info) << "Finish to load builtin node information";

    }

}
