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
#include <utility>

export module blueprint.builtin_node;
import blueprint.dyn_node;
import blueprint.plugin;
export import :signed_integral;
export import :unsigned_integral;
export import :float_point;
export import :matrix;
export import :identity_node;
export import :integral_arithmetic;
export import :float_arithmetic;
export import :matrix_editor;
export import :image;
export import :load_image;
export import :display_image;
export import :image_resize;
export import :image_brightness;
export import :image_kernel;
export import :image_decompose;

namespace blueprint::dyn_node::builtin
{

    template <type_definition T>
    type_definition_proxy def_type()
    {
        return std::make_shared<T>();
    }
    template <node_definition T, typename... Args>
    node_definition_proxy def_node(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <type_definition T>
    std::pair<type_definition_proxy, id_type> def_type_id()
    {
        auto def = def_type<T>();
        auto id = def->id();
        return {std::move(def), id};
    }

    template <node_definition T, typename... Args>
    std::pair<node_definition_proxy, id_type> def_node_id(Args&&... args)
    {
        auto def = def_node<T>(std::forward<Args>(args)...);
        auto id = def->id();
        return {std::move(def), id};
    }

    export plugin::component_package builtin_components()
    {
        using namespace blueprint::builtin;
        dyn_node::data_proxy p = std::make_shared<builtin_image_t>();

        using std::make_shared;
        using std::make_unique;

        auto signed_int = def_type<sint_definition>();
        auto unsigned_int = def_type<uint_definition>();
        auto [float_point, float_point_id] = def_type_id<float_definition>();
        auto signed_id = signed_int->id();
        auto unsigned_id = unsigned_int->id();

        auto matrix_def = def_type<matrix_definition>();
        auto image_def = def_type<image_definition>();
        auto matrix_id = matrix_def->id();
        auto image_id = image_def->id();

        auto signed_node = def_node<identity_node_definition>(signed_int);
        auto unsigned_node = def_node<identity_node_definition>(unsigned_int);
        auto [float_point_node, float_point_node_id] = def_node_id<identity_node_definition>(float_point);
        auto signed_node_id = signed_node->id();
        auto unsigned_node_id = unsigned_node->id();

        auto signed_add = def_node<sint_plus_node>();
        auto unsigned_add = def_node<uint_plus_node>();
        auto signed_sub = def_node<sint_sub_node>();
        auto unsigned_sub = def_node<uint_sub_node>();
        auto signed_mul = def_node<sint_mul_node>();
        auto unsigned_mul = def_node<uint_mul_node>();
        auto signed_plus_id = signed_add->id();
        auto unsigned_plus_id = unsigned_add->id();
        auto signed_sub_id = signed_sub->id();
        auto unsigned_sub_id = unsigned_sub->id();
        auto signed_mul_id = signed_mul->id();
        auto unsigned_mul_id = unsigned_mul->id();

        auto [float_add, float_add_id] = def_node_id<float_plus_node>();
        auto [float_sub, float_sub_id] = def_node_id<float_minus_node>();
        auto [float_mul, float_mul_id] = def_node_id<float_mul_node>();
        auto [float_div, float_div_id] = def_node_id<float_div_node>();
        auto [float_mod, float_mod_id] = def_node_id<float_mod_node>();

        auto matrix_editor = def_node<matrix_editor_def>();
        auto matrix_display = def_node<matrix_display_def>();
        auto matrix_editor_id = matrix_editor->id();
        auto matrix_display_id = matrix_display->id();

        auto load_image = def_load_image_node();
        auto load_image_id = load_image->id();
        auto display_image = def_node<display_image_definition>();
        auto display_image_id = display_image->id();

        auto img_resize = def_image_resize_node();
        auto img_resize_id = img_resize->id();
        auto img_brightness = def_image_brightness_node();
        auto img_brightness_id = img_brightness->id();
        auto img_kernel = def_image_kernel_node();
        auto img_kernel_id = img_kernel->id();
        auto img_decompose = def_image_decompose_node();
        auto img_decompose_id = img_decompose->id();

        return {
            .types = {
                {
                    std::move(signed_int),
                    std::move(unsigned_int),
                    std::move(matrix_def),
                    std::move(image_def),
                    std::move(float_point),
                }
            },
            .nodes = {
                {
                    std::move(signed_node),
                    std::move(unsigned_node),
                    std::move(float_point_node),
                    std::move(signed_add),
                    std::move(unsigned_add),
                    std::move(signed_sub),
                    std::move(unsigned_sub),
                    std::move(signed_mul),
                    std::move(unsigned_mul),
                    std::move(matrix_editor),
                    std::move(matrix_display),
                    std::move(load_image),
                    std::move(display_image),
                    std::move(float_add),
                    std::move(float_sub),
                    std::move(float_mul),
                    std::move(float_div),
                    std::move(float_mod),
                    std::move(img_resize),
                    std::move(img_brightness),
                    std::move(img_kernel),
                    std::move(img_decompose),
                }
            },
            .groups = {
                {
                    "Core", {
                        signed_node_id, unsigned_node_id,
                        float_point_node_id
                    }
                },
                {
                    "Arithmetic", {
                        signed_plus_id, unsigned_plus_id,
                        signed_sub_id, unsigned_sub_id,
                        signed_mul_id, unsigned_mul_id,
                        float_add_id,
                        float_sub_id,
                        float_mul_id,
                        float_div_id,
                        float_mod_id,
                    }
                },
                {
                    "Matrix", {
                        matrix_editor_id,
                        matrix_display_id,
                    }
                },
                {
                    "Image", {
                        load_image_id,
                        display_image_id,
                        img_resize_id,
                        img_brightness_id,
                        img_kernel_id,
                        img_decompose_id,
                    }
                }
            },
        };
    }

}
