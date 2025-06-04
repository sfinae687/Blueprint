//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/2.
//
module;

#include <proxy/proxy.h>

export module blueprint.builtin_node:float_point;
import blueprint.dyn_node;
import blueprint.stk_node;

namespace blueprint::builtin {
    export constexpr char float_id[] = "core.float.double";
    export using builtin_float = double;
    struct builtin_float_hint {};
}

namespace blueprint::dyn_node
{
    template<>
    struct lookup_hint<builtin::builtin_float>
    {
        using type = builtin::builtin_float_hint;
    };
}

namespace blueprint::builtin
{
    constexpr char float_name[] = "Float";
    constexpr char float_description[] = "Double precision float number";

    export using float_definition = stk_node::template_type_definition<float_id, float_name, float_description>;

    export dyn_node::id_type type_id(builtin_float_hint, builtin_float)
    {
        return float_id;
    }
    export dyn_node::data_proxy clone(builtin_float_hint, builtin_float)
    {
        return std::make_shared<builtin_float>(0.);
    }

    static_assert(dyn_node::data<builtin_float>);
}