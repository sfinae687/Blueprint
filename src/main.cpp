// Copyright (c) 2025 sfinea687
// Licensed under the MIT License. See LICENSE in the project root for details.

import blueprint.gui;

#include "imgui.h"

#include <boost/log/utility/setup.hpp>

#include <iostream>

void init_log()
{
    boost::log::add_console_log(std::clog);

}

int main(int argc, char *argv[])
{

    init_log();

    blueprint::GUI::init_gui();

    blueprint::GUI::window wd("Hello", 640, 480);
    wd.set_draw_op([&]
    {
        ImGui::Text("Hello Gui.");
    });
    wd.render_loop();

    blueprint::GUI::final_gui();
    return 0;
}
