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
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

module blueprint;
import blueprint.gui;
import blueprint.dyn_node;
import blueprint.draw_node;
import blueprint.builtin_node;

namespace blueprint
{

    namespace ranges = std::ranges;
    namespace views = std::views;
    using namespace std::string_literals;

    /// Constructor, it defines the all GUI, and connect another necessary component.
    blueprint_application::blueprint_application()
        : gui_("Blueprint Node editor", 720, 360)
        , imnodes_context_(gui_)
        , link_(node_instance_)
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

            for (auto &&hd : node_instance_.dump_handler())
            {
                draw_node(hd);
            }

            draw_editor_menu();

            ImNodes::EndNodeEditor();

            ImGui::EndChild();

            ImGui::Text("hello world");
        }
        ImGui::End();

    }

    void blueprint_application::draw_node(flow::node_instance_handler hd)
    {
        namespace ranges = std::ranges;
        namespace views = std::views;
        using namespace blueprint::dyn_node;

        auto &&p = hd.node_instance();
        const auto id = hd.node_id();
        const auto type_id = p->type_id();
        const auto name = node_def_[type_id]->name();
        const auto [inputs, outputs] = util::current_signature(p);

        ImNodes::BeginNode(id);

        // Title

        // const auto title_id = std::format("NodeTitle##{}", type_id);
        ImGui::Text(name.data());

        // Attribute

        for (auto &&[ind, ip] : inputs | views::enumerate)
        {
            auto input_id = flow::input_channel_id(id, ind);
            ImNodes::BeginInputAttribute(input_id);
            ImNodes::EndInputAttribute();
        }

        for (auto &&[ind, ip] : outputs | views::enumerate)
        {
            auto output_id = flow::output_channel_id(id, ind);
            ImNodes::BeginOutputAttribute(output_id);

            ImNodes::EndOutputAttribute();
        }

        // Node Content

        ImNodes::EndNode();

    }


    void blueprint_application::load_builtin()
    {
        BOOST_LOG_SEV(logger, info) << "Loading builtin Components";
        load_component(dyn_node::builtin::builtin_components());


        BOOST_LOG_SEV(logger, info) << "Loading builtin Nodes draw rule";
        node_draw_ = draw_node::builtin_node_draw_map();

        BOOST_LOG_SEV(logger, info) << "Loading builtin Types draw rule";
        type_draw_ = draw_node::builtin_type_draw_map();

        BOOST_LOG_SEV(logger, info) << "Finish to load builtin node information";
    }
    void blueprint_application::load_component(plugin::component_package pkg)
    {
        for (auto ty : pkg.types | views::as_rvalue)
        {
            if (type_def_.contains(ty->id()))
            {
                BOOST_LOG_SEV(logger, error) << "Load a type has be loaded. The later definition will be ignored";
            }
            else
            {
                auto id = ty->id();
                type_def_[id] = std::move(ty);
            }
        }

        for (auto nd : pkg.nodes | views::as_rvalue)
        {
            if (node_def_.contains(nd->id()))
            {
                BOOST_LOG_SEV(logger, error) << "Load a node has be loaded. The later definition will be ignored";
            }
            else
            {
                auto id = nd->id();
                node_def_[id] = std::move(nd);
            }
        }

        for (auto [name, ids] : pkg.groups | views::as_rvalue)
        {
            std::string cname{name.begin(), name.end()};
            auto&& cur_list = menu_def_[cname];
            for (auto id : ids)
            {
                if (cur_list.contains(id))
                {
                    BOOST_LOG_SEV(logger, error) << "Create a menu entry has exists";
                }
                else
                {
                    cur_list.insert(id);
                }
            }
        }
    }
    void blueprint_application::draw_editor_menu()
    {
        if (ImNodes::IsEditorHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup(editor_menu_id.data());
        }

        if (ImGui::BeginPopup(editor_menu_id.data()))
        {
            for (auto&& [group_name, members] : menu_def_)
            {
                std::string group_menu_id = group_name + "##GroupMenu";
                if (ImGui::BeginMenu(group_menu_id.c_str()))
                {
                    for (auto&& ent : members)
                    {
                        auto node_name = node_def_[ent]->name();
                        auto item_id = node_name.data() + "##"s + group_menu_id;
                        if (ImGui::MenuItem(item_id.c_str()))
                        {
                            BOOST_LOG_SEV(logger, trace) << "Create node: " << ent;
                            to_create_node(ent);
                        }
                    }
                    ImGui::EndMenu();
                }
            }
            ImGui::EndPopup();
        }
    }
    void blueprint_application::to_remove_node(flow::no_id)
    {

    }

    void blueprint_application::to_create_node(dyn_node::id_type id)
    {
        auto &&def = node_def_[id];
        auto new_instance = def->create_node();
        node_instance_.add_instance(std::move(new_instance));
    }

} // namespace blueprint
