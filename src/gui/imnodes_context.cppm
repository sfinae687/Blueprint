//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-28.
//

/// @file
/// The class manages the imnodes context.

module;
#include <imnodes.h>

export module blueprint.gui:imnode_context;
import :forward_declaration;

namespace blueprint::GUI
{

    // Forward declaration


    /**
     * The class manages the imnodes context. It is constructed with a GUI::window, and initialize an imnodes context
     * within the GUI::window context, and destroy the context when the object destructed.
     */
    export class imnodes_context
    {
    public:
        explicit imnodes_context(window &w);
        imnodes_context(const imnodes_context &) = delete;
        imnodes_context& operator= (const imnodes_context &) = delete;
        ~imnodes_context();

        [[nodiscard]] ImNodesContext* get_context() const;

        void activate_context();

        void inactivate_context();

    private:
        GUI::window &window_;
        ImNodesContext *context_;
    };


}
