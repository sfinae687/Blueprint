//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/5/31.
//

module;

#include <Eigen/Dense>

#include <memory>
#include <string_view>

export module blueprint.builtin_node:matrix_editor;
import blueprint.dyn_node;
import :matrix;

namespace blueprint::builtin
{
    using dyn_node::data_sequence;
    using dyn_node::signature_t;
    using dyn_node::node_instance_proxy;

    export constexpr char matrix_editor_id[] = "core.matrix-editor";

    export class matrix_editor_def
    {
    public:
        id_type id() const noexcept;
        text_type name() const noexcept;
        text_type description() const noexcept;

        node_instance_proxy create_node() noexcept;
    };

    export class matrix_editor_node
    {
    public:
        matrix_editor_node();


        [[nodiscard]] id_type type_id() const noexcept;
        [[nodiscard]] std::span<const signature_t> signatures() const noexcept;
        [[nodiscard]] std::size_t current_variant() const noexcept;
        bool set_variant(std::size_t) noexcept;
        bool compute(data_sequence) noexcept;
        [[nodiscard]] data_sequence output() const noexcept;

        std::shared_ptr<builtin_matrix_t> mat_;
    };
}
