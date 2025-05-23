//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-26.
//


module;
#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imnodes.h>

module blueprint;
import blueprint.gui;

namespace blueprint
{

    /// Constructor, it defines the all GUI, and connect other necessary component.
    blueprint_application::blueprint_application()
        : gui_("Blueprint Node editor", 720, 360)
        , nodes_(gui_)
    {
        using namespace GUI;

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

    blueprint_application::~blueprint_application()
    = default;


    int blueprint_application::run()
    {
        gui_.render_loop();
        return 0;
    }
}
