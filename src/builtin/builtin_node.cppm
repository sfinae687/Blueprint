//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//
module;
#include <opencv2/core.hpp>
#include <proxy/proxy.h>

#include <memory>
#include <vector>
#include <string>
#include <string_view>

export module blueprint.builtin_node;
import blueprint.dyn_node;
import blueprint.plugin;
export import :signed_integral;
export import :unsigned_integral;
export import :matrix;
export import :identity_node;
export import :integral_arithmetic;
export import :matrix_editor;
export import :image;

namespace blueprint::dyn_node::builtin
{

    template <dyn_node::type_definition T>
    type_definition_proxy make_type_def()
    {
        return std::make_shared<T>();
    }
    template <dyn_node::node_definition T, typename... Args>
    node_definition_proxy make_node_def(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    export plugin::component_package builtin_components()
    {
        using namespace blueprint::builtin;
        dyn_node::data_proxy p = std::make_shared<builtin_image_t>();

        using std::make_shared;
        using std::make_unique;

        auto signed_int = make_type_def<sint_definition>();
        auto unsigned_int = make_type_def<uint_definition>();
        auto signed_id = signed_int->id();
        auto unsigned_id = unsigned_int->id();

        auto matrix_def = make_type_def<matrix_definition>();
        auto image_def = make_type_def<image_definition>();
        auto matrix_id = matrix_def->id();
        auto image_id = image_def->id();

        auto signed_node = make_node_def<identity_node_definition>(signed_int);
        auto unsigned_node = make_node_def<identity_node_definition>(unsigned_int);
        auto signed_node_id = signed_node->id();
        auto unsigned_node_id = unsigned_node->id();

        auto signed_add = make_node_def<sint_plus_node>();
        auto unsigned_add = make_node_def<uint_plus_node>();
        auto signed_sub = make_node_def<sint_sub_node>();
        auto unsigned_sub = make_node_def<uint_sub_node>();
        auto signed_mul = make_node_def<sint_mul_node>();
        auto unsigned_mul = make_node_def<uint_mul_node>();
        auto signed_plus_id = signed_add->id();
        auto unsigned_plus_id = unsigned_add->id();
        auto signed_sub_id = signed_sub->id();
        auto unsigned_sub_id = unsigned_sub->id();
        auto signed_mul_id = signed_mul->id();
        auto unsigned_mul_id = unsigned_mul->id();

        auto matrix_editor = make_node_def<matrix_editor_def>();
        auto matrix_display = make_node_def<matrix_display_def>();
        auto matrix_editor_id = matrix_editor->id();
        auto matrix_display_id = matrix_display->id();

        return {
            .types = {
                {
                    std::move(signed_int),
                    std::move(unsigned_int),
                    std::move(matrix_def),
                    std::move(image_def),
                }
            },
            .nodes = {
                {
                    std::move(signed_node),
                    std::move(unsigned_node),
                    std::move(signed_add),
                    std::move(unsigned_add),
                    std::move(signed_sub),
                    std::move(unsigned_sub),
                    std::move(signed_mul),
                    std::move(unsigned_mul),
                    std::move(matrix_editor),
                    std::move(matrix_display),
                }
            },
            .groups = {
                {
                    "Core", {signed_node_id, unsigned_node_id}
                },
                {
                    "Arithmetic", {
                        signed_plus_id, unsigned_plus_id,
                        signed_sub_id, unsigned_sub_id,
                        signed_mul_id, unsigned_mul_id
                    }
                },
                {
                    "Matrix", {
                        matrix_editor_id,
                        matrix_display_id,
                    }
                }
            },
        };
    }

}
