//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-26.
//


module;
#include <imgui.h>

module blueprint;
import blueprint.gui;

namespace blueprint
{
    blueprint_application::blueprint_application()
        : gui("Blueprint Node editor", 720, 360)
    {

        using namespace GUI;

        gui.set_draw_operation([]
        {
            bool main_open;
            if (begin_main_window("main", &main_open))
            {
                ImGui::Text("hello world");
            }
            ImGui::End();

            ImGui::ShowDemoWindow();
        });
    }

    int blueprint_application::run()
    {
        gui.render_loop();
        return 0;
    }
}
