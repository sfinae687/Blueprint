//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-26.
//

module;
#include <imgui.h>
#include <thread_pool/thread_pool.h>

#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/scope/scope_exit.hpp>
#include <boost/lockfree/queue.hpp>

#include <map>
#include <unordered_set>
#include <set>
#include <queue>


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
        using self = blueprint_application;
        using application_logger_t =
            boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>;
        using enum boost::log::trivial::severity_level;

        static constexpr std::string_view editor_menu_id = "EditorMenu";

        struct new_node_context
        {
            ImVec2 pos;
        };
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
        draw_node::data_draw_context& make_draw_context(flow::no_id);
        void process_link();
        void draw_link();
        void draw_editor_menu();
        /* TODO Implement Me */ void draw_node_menu();



        void to_create_link(flow::no_id output, flow::no_id input);
        void to_remove_node(flow::no_id);
        void do_remove_node(flow::no_id);
        void to_create_node(dyn_node::id_type, new_node_context);
        void do_create_node(dyn_node::id_type id, new_node_context ctx);
        void to_computing(flow::no_id);
        void to_finish_computing(flow::no_id, bool);
        void do_finish_computing(flow::no_id, bool);
        void to_switch_variant(flow::no_id, std::size_t i);


        // logger
        application_logger_t logger{};

        // context manager

        GUI::window gui_;
        GUI::imnodes_context imnodes_context_;
        int hovered_node = -1;

        // application state
        bool main_open = true;

        // Node And Types definition
        dyn_node::node_definitions_t node_def_;
        dyn_node::type_definitions_t type_def_;

        // Draw Rule for Node and Type
        draw_node::node_draw_map_t node_draw_{};
        draw_node::type_draw_map_t type_draw_{};

        std::map<flow::no_id, draw_node::data_draw_context>
        draw_contexts_;

        // Node and link instance
        flow::node_instance_manager node_instance_;
        constraint::constraint_flow link_;

        std::unordered_map<flow::no_id, new_node_context> new_node_;

        // Menu definition
        using menu_def_t = std::map<std::string, std::set<std::string_view>>;
        menu_def_t menu_def_;

        // Update flag
        bool begin_compute_flag = false;

        // event queue
        std::queue<flow::no_id> to_remove_nodes_;
        std::queue<std::pair<dyn_node::id_type, new_node_context>> to_create_nodes_;
        std::unordered_set<flow::no_id> in_computing_;

        struct compute_result
        {
            flow::no_id node_id;
            bool success;
        };
        boost::lockfree::queue<compute_result> to_finish_compute_;

        // executor
        dp::thread_pool<> executor_;

    };


}
