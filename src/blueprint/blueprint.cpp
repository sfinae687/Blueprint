//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-26.
//


module;
#define IMGUI_DEFINE_MATH_OPERATORS

#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <imgui.h>
#include <imnodes.h>
#include <proxy.h>

#include <iomanip>
#include <ranges>

module blueprint;
import blueprint.gui;
import blueprint.dyn_node;
import blueprint.draw_node;
import blueprint.builtin_node;

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

        gui_.set_update_operation([this]() {update();});
        gui_.set_draw_operation([this]() {draw();});
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

    void blueprint_application::update()
    {

    }
    void blueprint_application::draw()
    {
        using namespace GUI;
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

    }

    void blueprint_application::draw_nodes(dyn_node::node_instance_proxy p, std::size_t id)
    {
        namespace ranges = std::ranges;
        namespace views = std::views;
        using namespace blueprint::dyn_node;

        constexpr std::size_t offset_num = 3;
        constexpr std::size_t id_bit = sizeof(id) * CHAR_BIT;
        constexpr std::size_t id_upper_bounder = (std::size_t{1} << (id_bit - offset_num));

        auto tid = p->type_id();
        auto [inputs, outputs] = util::current_signature(p);

        if (id >= id_upper_bounder)
        {
            BOOST_LOG_SEV(logger, warning)
            << "The id of a noded of " << std::quoted(tid) << "type is too large. It may causes unexpected behavior.";
        }
        if (inputs.size() + outputs.size() >= (std::size_t{1} << offset_num))
        {
            BOOST_LOG_SEV(logger, warning)
            << "The number of channels of " << std::quoted(tid) << " is too large. It may causes unexpected behavior.";
        }

        for (auto &&[ind, ip] : inputs | views::enumerate)
        {
            auto iid = (id << offset_num) + ind;
            ImNodes::BeginInputAttribute(iid);
            // type_draw_[ip]()
            ImNodes::EndInputAttribute();
        }

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
