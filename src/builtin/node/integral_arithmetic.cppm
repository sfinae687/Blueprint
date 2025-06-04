//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-30.
//

module;

#include <proxy/proxy.h>

#include <functional>

export module blueprint.builtin_node:integral_arithmetic;
import blueprint.stk_node;
import blueprint.dyn_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    using stk_node::deduced_func_node;
    using sint_op_base = deduced_func_node<builtin_mapper, builtin_signed_type, builtin_signed_type, builtin_signed_type>;

    using uint_op_base = deduced_func_node<builtin_mapper, builtin_unsigned_type, builtin_unsigned_type, builtin_unsigned_type>;

    constexpr char signed_plus_id[] = "core.arithmetic.plus.signed";

    export class sint_plus_node : public sint_op_base
    {
    public:
        sint_plus_node()
            : sint_op_base(std::plus<builtin_signed_type>{},
                "Plus(Signed)", "Process addition of two signed integral number.",
                signed_plus_id
            )
        {}
    };

    constexpr char unsigned_plus_id[] = "core.arithmetic.plus.unsigned";

    export class uint_plus_node : public uint_op_base
    {
    public:
        uint_plus_node()
            : uint_op_base(std::plus<builtin_unsigned_type>{},
                "Plus(Unsigned)", "Process addition of two unsigned integral number",
                unsigned_plus_id
            )
        {}
    };

    constexpr char signed_sub_id[] = "core.arithmetic.minus.signed";

    export class sint_sub_node : public sint_op_base {
    public:
        sint_sub_node()
            : sint_op_base(
                std::minus<builtin_signed_type>{},
                "Minus(Signed)", "Process substraction of two signed integral number",
                signed_sub_id
            )
        {}
    };

    constexpr char unsigned_sub_id[] = "core.arithmetic.minus.unsigned";

    export class uint_sub_node : public uint_op_base {
    public:
        uint_sub_node()
            : uint_op_base(
                std::minus<builtin_unsigned_type>{},
                "Minus(Unsigned)", "Process substraction of two unsigned integral number",
                unsigned_sub_id
            )
        {}
    };

    constexpr char signed_mul_id[] = "core.arithmetic.multiply.signed";

    export class sint_mul_node : public sint_op_base {
    public:
        sint_mul_node()
            : sint_op_base(
                std::multiplies<builtin_signed_type>{},
                "Multiplies(Signed)", "Process multiplies of two signed integral number",
                signed_mul_id
            )
        {}
    };

    constexpr char unsigned_mul_id[] = "core.arithmetic.multiply.unsigned";

    export class uint_mul_node : public uint_op_base {
    public:
        uint_mul_node()
            : uint_op_base(
                std::multiplies<builtin_unsigned_type>{},
                "Multiplies(Unsigned)", "Process multiplies of two unsigned integral number",
                unsigned_mul_id
            )
        {}
    };

}
