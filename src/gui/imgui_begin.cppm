//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-24.
//

/// @file
/// Provide some function calling ImGui::Begin with predefined config set.

module;
#include <string_view>

export module blueprint.gui:begin;

namespace blueprint::GUI
{
    /**
     * Begin a window fill all native viewport.
     */
    export bool begin_main_window(std::string_view title, bool* p_open) noexcept;
}