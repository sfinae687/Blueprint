//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-28.
//

module;
#include <imnodes.h>

module blueprint.gui;
import :window;

namespace blueprint::GUI
{
    imnodes_context::imnodes_context(window& w)
        : window_(w)
    {
        w.entry([&]
        {
            this->context_ = ImNodes::CreateContext();
        });
    }

    imnodes_context::~imnodes_context()
    {
        window_.entry([&]
        {
           ImNodes::DestroyContext(context_);
        });
    }

    ImNodesContext* imnodes_context::get_context() const
    {
        return context_;
    }

    void imnodes_context::activate_context()
    {
        ImNodes::SetCurrentContext(context_);
    }

    void imnodes_context::inactivate_context()
    {
        ImNodes::SetCurrentContext(nullptr);
    }
}