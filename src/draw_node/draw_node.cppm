//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//

export module blueprint.draw_node;
import blueprint.builtin_node;
export import :draw_rule;
export import :builtin;

namespace blueprint::draw_node
{
    // utility

    export type_draw_map_t builtin_type_draw_map()
    {
        using namespace dyn_node;
        using namespace dyn_node::builtin;

        type_draw_map_t rt;
        rt[SIGNED_INTEGRAL_ID] = &draw_signed;
        rt[UNSIGNED_INTEGRAL_ID] = &draw_unsigned;
        rt[matrix_id] = &draw_matrix;

        return rt;
    }

    export node_draw_map_t builtin_node_draw_map()
    {
        using namespace dyn_node;
        using namespace dyn_node::builtin;
        using namespace blueprint::builtin;

        node_draw_map_t rt;
        rt[matrix_editor_id] = &draw_matrix_editor;
        return rt;
    }

}
