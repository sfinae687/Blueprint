//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-24.
//

module;
#include <imgui.h>
#include <string_view>

#include <iostream>

module blueprint.gui;
import :begin;

namespace blueprint::GUI
{
    bool begin_main_window(std::string_view title, bool* p_open) noexcept
    {
        const auto vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->Pos);
        ImGui::SetNextWindowSize(vp->Size);

        constexpr auto main_win_flag = ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_MenuBar
            | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoDecoration;
        return ImGui::Begin(title.data(), p_open, main_win_flag);
    }
}