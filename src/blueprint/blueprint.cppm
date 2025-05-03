//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-26.
//

module;
#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/scope/scope_exit.hpp>

export module blueprint;
import blueprint.gui;
import blueprint.dyn_node;
import blueprint.draw_node;

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
    public:
        blueprint_application();
        ~blueprint_application() = default;

        blueprint_application(const blueprint_application &) = delete;
        blueprint_application& operator= (const blueprint_application &) = delete;

        int run();

    private:
        void setup_logger();
        void load_builtin();

        // logger
        application_logger_t logger{};

        // context manager

        GUI::window gui_;
        GUI::imnodes_context nodes_;

        // application state
        bool main_open = true;

        // Node And Types
        dyn_node::node_definitions_t node_def_;
        dyn_node::type_definitions_t type_def_;

        // Draw Node and Type
        draw_node::node_draw_map_t node_draw_;
        draw_node::type_draw_map_t type_draw_;
    };


}
