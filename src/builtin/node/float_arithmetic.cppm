//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/2.
//

module;

#include <boost/core/functor.hpp>

#include <functional>
#include <cmath>

export module blueprint.builtin_node:float_arithmetic;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;
import :float_point;

namespace blueprint::builtin
{
    using float_op_base = stk_node::deduced_func_node<builtin_mapper, builtin_float, builtin_float, builtin_float>;

    export
    {
        constexpr char float_plus_id[] = "core.arithmetic.plus.float";

        struct float_plus_node : float_op_base
        {
            float_plus_node()
                : float_op_base(std::plus<builtin_float>{},
                    "Plus(Float)", "Process plus of two floating-point number",
                    float_plus_id
                )
            {}
        };

        constexpr char float_sub_id[] = "core.arithmetic.minus.float";

        struct float_minus_node : float_op_base
        {
            float_minus_node()
                : float_op_base(std::minus<builtin_float>{},
                    "Minus(Float)", "Process minus of two floating-point number",
                    float_plus_id
                )
            {}
        };

        constexpr char float_mul_id[] = "core.arithmetic.multiples.float";

        struct float_mul_node : float_op_base
        {
            float_mul_node()
                : float_op_base(std::multiplies<builtin_float>{},
                    "Multiples(Float)", "Process multiples of two floating-point number",
                    float_mul_id
                )
            {}
        };

        constexpr char float_div_id[] = "core.arithmetic.divides.float";

        struct float_div_node : float_op_base
        {
            float_div_node()
                : float_op_base(std::divides<builtin_float>{},
                    "Divides(Float)", "Process divides of two floating-point number",
                    float_div_id
                )
            {}
        };

        constexpr char float_mod_id[] = "core.arithmetic.modulus.float";

        struct float_mod_node : float_op_base
        {
            float_mod_node()
                : float_op_base(&std::fmod<builtin_float, builtin_float>,
                    "Modulus(Float)", "Process divides of two floating-point, number",
                    float_mod_id
                )
            {}
        };
    }

}
