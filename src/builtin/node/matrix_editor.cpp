//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/5/31.
//

module;

#include <memory>

module blueprint.builtin_node;
import :matrix_editor;

namespace blueprint::builtin
{

    // Matrix Editor

    id_type matrix_editor_def::id() const noexcept
    {
        return matrix_editor_id;
    }

    text_type matrix_editor_def::name() const noexcept
    {
        return "Matrix editor";
    }

    text_type matrix_editor_def::description() const noexcept
    {
        return "The node output a small matrix";
    }

    node_instance_proxy matrix_editor_def::create_node() noexcept
    {
        return std::make_unique<matrix_editor_node>();
    }

    matrix_editor_node::matrix_editor_node()
        : mat_(std::make_shared<builtin_matrix_t>(0, 0))
    {}

    id_type matrix_editor_node::type_id() const noexcept
    {
        return matrix_editor_id;
    }
    std::span<const signature_t> matrix_editor_node::signatures() const noexcept
    {
        static signature_t sig {
            .input = {},
            .output = {matrix_id}
        };

        return {&sig, 1};
    }
    std::size_t matrix_editor_node::current_variant() const noexcept
    {
        return 0;
    }
    bool matrix_editor_node::set_variant(std::size_t) noexcept
    {
        return false;
    }
    bool matrix_editor_node::compute(data_sequence ds) noexcept
    {
        return ds.size() == 0;
    }
    data_sequence matrix_editor_node::output() const noexcept
    {
        return {mat_};
    }

    // Matrix Display

    id_type matrix_display_def::id() const noexcept
    {
        return matrix_display_id;
    }
    text_type matrix_display_def::name() const noexcept
    {
        return "Matrix Display";
    }
    text_type matrix_display_def::description() const noexcept
    {
        return "Display the matrix inputted";
    }

    node_instance_proxy matrix_display_def::create_node() noexcept
    {
        return std::make_unique<matrix_display_node>();
    }
    id_type matrix_display_node::type_id() const noexcept
    {
        return matrix_display_id;
    }
    std::size_t matrix_display_node::current_variant() const noexcept
    {
        return 0;
    }
    bool matrix_display_node::set_variant(std::size_t) noexcept
    {
        return false;
    }
    std::span<const signature_t> matrix_display_node::signatures() const noexcept
    {
        static dyn_node::signature_t sig{
            .input = {matrix_id},
            .output =  {}
        };

        return {&sig, 1};
    }
    bool matrix_display_node::compute(data_sequence ds) noexcept
    {
        return ds.size() == 1;
    }
    data_sequence matrix_display_node::output() const noexcept
    {
        return {};
    }

} // namespace blueprint::builtin
