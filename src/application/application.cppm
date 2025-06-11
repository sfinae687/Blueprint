//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/11.
//

module;

#include <boost/log/trivial.hpp>
#include <boost/scope/scope_exit.hpp>

export module application;
import blueprint;
import blueprint.gui;

namespace application
{
    using namespace blueprint;

    export auto app_guard()
    {
        GUI::init_gui();
        return boost::scope::scope_exit([]
        {
            GUI::finish_gui();
        });
    }

    export class application_context
    {
        using self = blueprint_application;
        using application_logger_t =
            boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>;
        using enum boost::log::trivial::severity_level;

    public:

        application_context();
        ~application_context();

        application_context(const application_context &) = delete;
        application_context& operator= (const application_context &) = delete;

        int run();

    private:
        void setup_logger();
        void update();
        void draw();


        application_logger_t logger{};

        // context manager
        GUI::window gui_;
        blueprint_application *editors_ = nullptr;

        // application state
        bool main_open = true;

    };

}
