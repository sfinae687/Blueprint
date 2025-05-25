//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-26.
//

module;
#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/scope/scope_exit.hpp>
#include <map>
#include <unordered_set>

export module blueprint;
import blueprint.gui;
import blueprint.dyn_node;
import blueprint.draw_node;
import blueprint.plugin;
import blueprint.flow;
import blueprint.constraint;

namespace blueprint
{

    export auto app_guard()
    {
        GUI::init_gui();
        return boost::scope::scope_exit([]
        {
            GUI::finish_gui();
        });
    }

    export class blueprint_application
    {
        using application_logger_t =
            boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>;
        using enum boost::log::trivial::severity_level;

        static constexpr std::string_view editor_menu_id = "EditorMenu";
    public:
        blueprint_application();
        ~blueprint_application() = default;

        blueprint_application(const blueprint_application &) = delete;
        blueprint_application& operator= (const blueprint_application &) = delete;

        int run();

    private:
        void setup_logger();
        void update();
        void draw();

        void load_builtin();
        void load_component(plugin::component_package);

        void draw_node(flow::node_instance_handler hd);
        void draw_editor_menu();
        draw_node::data_draw_context& make_draw_context(flow::no_id);

        void to_remove_node(flow::no_id);
        void to_create_node(dyn_node::id_type);


        // logger
        application_logger_t logger{};

        // context manager

        GUI::window gui_;
        GUI::imnodes_context imnodes_context_;

        // application state
        bool main_open = true;

        // Node And Types definition
        dyn_node::node_definitions_t node_def_;
        dyn_node::type_definitions_t type_def_;

        // Draw Rule for Node and Type
        draw_node::node_draw_map_t node_draw_;
        draw_node::type_draw_map_t type_draw_;

        std::map<flow::no_id, draw_node::data_draw_context>
        draw_contexts_;

        // Node and link instance
        flow::node_instance_manager node_instance_;
        constraint::constraint_flow link_;

        // Menu definition
        using menu_def_t = std::unordered_map<std::string, std::unordered_set<std::string_view>>;
        menu_def_t menu_def_;

    };


}
