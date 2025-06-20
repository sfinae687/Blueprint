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
export import :identity_node;
export import :integral_arithmetic;
export import :float_arithmetic;

export import :matrix;
export import :matrix_editor;
export import :matrix_arithmetic;

export import :image;
export import :load_image;
export import :display_image;
export import :image_resize;
export import :image_kernel;
export import :image_decompose;
export import :image_color_split;
export import :image_compose;
export import :image_weighted;
export import :image_alpha_beta;
export import :image_grayscale;
export import :image_blur;
export import :image_canny;
export import :to_matrix;
export import :to_image;

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
        auto img_kernel = def_image_kernel_node();
        auto img_kernel_id = img_kernel->id();
        auto img_decompose = def_image_decompose_node();
        auto img_decompose_id = img_decompose->id();
        auto img_split = def_image_color_split_node();
        auto img_split_id = img_split->id();
        auto img_compose = def_image_compose_node();
        auto img_compose_id = img_compose->id();
        auto img_add = def_image_weighted_node();
        auto img_add_id = img_add->id();
        auto img_alpha_beta = def_image_alpha_beta_node();
        auto img_alpha_beta_id = img_alpha_beta->id();
        auto img_grayscale = def_image_grayscale_node();
        auto img_grayscale_id = img_grayscale->id();
        auto img_blur = def_image_blur_node();
        auto img_blur_id = img_blur->id();
        auto img_gaussian_blur = def_image_gaussian_blur_node();
        auto img_gaussian_blur_id = img_gaussian_blur->id();
        auto img_canny = def_image_canny_node();
        auto img_canny_id = img_canny->id();
        auto img_to_matrix = def_image_to_matrix();
        auto to_matrix_id = img_to_matrix->id();

        auto mat_plus = def_matrix_plus();
        auto mat_plus_id = mat_plus->id();
        auto mat_sub = def_matrix_subtract();
        auto mat_sub_id = mat_sub->id();
        auto mat_mul = def_matrix_multiplies();
        auto mat_mul_id = mat_mul->id();
        auto mat_array_mul = def_matrix_array_multiplies();
        auto mat_array_mul_id = mat_array_mul->id();
        auto mat_scalar_plus = def_matrix_scalar_plus();
        auto mat_scalar_plus_id = mat_scalar_plus->id();
        auto mat_scalar_sub = def_matrix_scalar_subtract();
        auto mat_scalar_sub_id = mat_scalar_sub->id();
        auto mat_scalar_mul = def_matrix_scalar_multiplies();
        auto mat_scalar_mul_id = mat_scalar_mul->id();
        auto mat_to_img = def_matrix_to_img();
        auto to_img_id = mat_to_img->id();
        auto mat_neg = def_matrix_negate();
        auto mat_neg_id = mat_neg->id();
        auto mat_array_div = def_matrix_array_divide();
        auto mat_array_div_id = mat_array_div->id();
        auto mat_scalar_div = def_matrix_scalar_divide();
        auto mat_scalar_div_id = mat_scalar_div->id();
        auto mat_hypot = def_matrix_hypot();
        auto mat_hypot_id = mat_hypot->id();

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
                    std::move(img_kernel),
                    std::move(img_decompose),
                    std::move(img_split),
                    std::move(img_compose),
                    std::move(img_add),
                    std::move(img_alpha_beta),
                    std::move(img_grayscale),
                    std::move(img_blur),
                    std::move(img_gaussian_blur),
                    std::move(img_canny),
                    std::move(img_to_matrix),
                    std::move(mat_plus),
                    std::move(mat_sub),
                    std::move(mat_mul),
                    std::move(mat_array_mul),
                    std::move(mat_scalar_plus),
                    std::move(mat_scalar_sub),
                    std::move(mat_scalar_mul),
                    std::move(mat_to_img),
                    std::move(mat_neg),
                    std::move(mat_scalar_div),
                    std::move(mat_array_div),
                    std::move(mat_hypot),
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
                        mat_plus_id,
                        mat_sub_id,
                        mat_mul_id,
                        mat_array_mul_id,
                        mat_scalar_plus_id,
                        mat_scalar_sub_id,
                        mat_scalar_mul_id,
                        to_img_id,
                        mat_neg_id,
                        mat_array_div_id,
                        mat_scalar_div_id,
                        mat_hypot_id,
                    }
                },
                {
                    "Image", {
                        load_image_id,
                        display_image_id,
                        img_resize_id,
                        img_kernel_id,
                        img_add_id,
                        img_alpha_beta_id,
                        img_blur_id,
                        img_gaussian_blur_id,
                        img_canny_id,
                        to_matrix_id,
                    }
                },
                {
                    "Color Space", {
                        img_decompose_id,
                        img_split_id,
                        img_compose_id,
                        img_grayscale_id,
                    }
                }
            },
        };
    }

}
