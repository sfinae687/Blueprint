//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-26.
//

module;
#include <boost/scope/scope_exit.hpp>

export module blueprint;
import blueprint.gui;

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
    public:
        blueprint_application();
        ~blueprint_application();

        blueprint_application(const blueprint_application &) = delete;
        blueprint_application& operator= (const blueprint_application &) = delete;

        int run();

    private:

        // context manager

        GUI::window gui_;
        GUI::imnodes_context nodes_;

        // application state
        bool main_open = true;
    };


}
