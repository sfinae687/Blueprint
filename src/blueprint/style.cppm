//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-27.
//

module;

#include <imgui.h>

export module blueprint:style;
import blueprint.dyn_node;
import blueprint.flow;
import blueprint.constraint;

namespace blueprint
{
    export ImColor node_state_color(constraint::node_state ns)
    {
        using enum constraint::node_state;
        switch (ns)
        {
        case NOT_PREPARED:
            return {0xfefdca00};
        case DIRTY:
            return {0xffd46000};
        case COMPUTING:
            return {0xa1eafb00};
        case CLEAN:
            return {0xa6e3e900};
        case ERROR:
            return {0xe23e5700};
        default:
            return {0xf9f7f700};
        }
    }
}