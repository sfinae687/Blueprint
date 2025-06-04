//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//

module;

#include <filesystem>

export module blueprint.builtin_node:load_image;
import blueprint.dyn_node;

namespace blueprint::builtin
{
    export constexpr char load_image_node_id[] = "core.load-image";

    export dyn_node::node_definition_proxy def_load_image_node();
}
