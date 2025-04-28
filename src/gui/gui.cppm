// Copyright (c) 2025 sfinea687
// Licensed under the MIT License. See LICENSE in the project root for details.

/// @file
/// A naive encapsulation of ImGUI with glfw and OpenGL3 as backend.

export module blueprint.gui;

export import :debug;
export import :forward_declaration;
export import :begin;
export import :imnode_context;
export import :window;

namespace blueprint::GUI
{

    // Global operation

    /**
     * Execute the global scope initialization for gui.
     * @return true if success.
     */
    export bool init_gui();

    /**
     * Call the function after all gui operation finished.
     */
    export void finish_gui();

}