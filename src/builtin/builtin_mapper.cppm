//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-30.
//

module;

#include <concepts>

export module blueprint.builtin_node:builtin_mapper;
import blueprint.dyn_node;
import blueprint.stk_node;
export import :signed_integral;
export import :unsigned_integral;
export import :float_point;
export import :matrix;
export import :image;

namespace blueprint::builtin
{
    using namespace blueprint::dyn_node::builtin;

    export struct int_type_desc_t
        : stk_node::basic_type_desc<SIGNED_INTEGRAL_ID, builtin_signed_type, sint_definition>
    {
        template <std::signed_integral I>
        I transform(const builtin_signed_type &d) const
        {
            return static_cast<I>(auto{d});
        }

        template <std::signed_integral I>
        builtin_signed_type accept(I &&d) const
        {
            return static_cast<builtin_signed_type>(d);
        }
    };
    constexpr int_type_desc_t int_type_desc{};

    export struct uint_type_desc_t
        : stk_node::basic_type_desc<UNSIGNED_INTEGRAL_ID, builtin_unsigned_type, uint_definition>
    {
        template <std::unsigned_integral I>
        I transform(const builtin_unsigned_type &d) const
        {
            return static_cast<I>(auto{d});
        }
        template <std::unsigned_integral I>
        builtin_unsigned_type accept(I &&d) const
        {
            return static_cast<builtin_unsigned_type>(d);
        }
    };
    constexpr uint_type_desc_t uint_type_desc;

    export struct float_point_type_desc_t
        : stk_node::basic_type_desc<float_id, builtin_float, float_definition>
    {
        template <std::same_as<float> T>
        [[nodiscard]] T transform(const builtin_float &v) const
        {
            return v;
        }
        [[nodiscard]] builtin_float accept(float v) const
        {
            return builtin_float{v};
        }
    };
    constexpr float_point_type_desc_t float_point_type_desc;

    constexpr stk_node::basic_type_desc<matrix_id, builtin_matrix_t, matrix_definition> matrix_type_desc{};
    constexpr stk_node::basic_type_desc<image_id, builtin_image_t, image_definition> image_type_desc{};

    using builtin_mapper_t = stk_node::mapper<
        int_type_desc,
        uint_type_desc,
        float_point_type_desc,
        matrix_type_desc,
        image_type_desc
    >;
    constexpr builtin_mapper_t builtin_mapper{};
}