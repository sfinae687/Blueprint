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

    ImVec2 pos, sz;
    auto prev_time = std::chrono::system_clock::now();
    float frame_rate = 0;

    wd.set_update_operation([&]
    {
        using namespace std::chrono_literals;
        auto &io = ImGui::GetIO();
        sz = io.DisplaySize;
        pos = {0, 0};

        auto cur_time = std::chrono::system_clock::now();
        frame_rate = 1s / (cur_time - prev_time);
        prev_time = cur_time;

    });

    wd.set_draw_operation([&]
    {
        using namespace blueprint::GUI;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        constexpr auto main_win_flag = ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_MenuBar
            | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoDecoration;

        bool open;
        if (ImGui::Begin("Main", &open, main_win_flag))
        {
            ImGui::Text("Hello Gui.");
            ImGui::Text(std::to_string(frame_rate).c_str());
        }
        ImGui::End();

        ImGui::PopStyleVar();

        ImGui::ShowDemoWindow();
    });

    wd.render_loop();

    blueprint::GUI::final_gui();
    return 0;
}
