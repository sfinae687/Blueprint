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
    export constexpr char matrix_display_id[] = "core.matrix-display";

    export class matrix_editor_def;

    class matrix_editor_def
    {
    public:
        [[nodiscard]] id_type id() const noexcept;
        [[nodiscard]] text_type name() const noexcept;
        [[nodiscard]] text_type description() const noexcept;

        node_instance_proxy create_node() noexcept;

        node_instance_proxy load(archive::input_archive_t &);
        void save(archive::output_archive_t &, node_instance_proxy &p);
    };

    export class matrix_editor_node
    {
        friend class matrix_editor_def;
    public:
        matrix_editor_node();

        [[nodiscard]] id_type type_id() const noexcept;
        [[nodiscard]] std::span<const signature_t> signatures() const noexcept;
        [[nodiscard]] std::size_t current_variant() const noexcept;
        bool set_variant(std::size_t) noexcept;
        bool compute(data_sequence) noexcept;
        [[nodiscard]] data_sequence output() const noexcept;

        enum mat_type : int
        {
            Custom,
            Laplace_kernel,
            Vertical_sobel,
            Horizontal_sobel,
            Vertical_Prewitt,
            Horizontal_Prewitt,
            Gaussian_kernel,
            Zeros,
            Ones,
            Eye,
        };

        std::shared_ptr<builtin_matrix_t> mat_{};
        int cur_item = 0;

        struct gaussian_kernel_context
        {
            int sz;
            double gamma;
        };
        struct size_context
        {
            int height;
            int width;
        };
        struct eye_context
        {
            int sz;
        };
        std::variant<std::monostate, gaussian_kernel_context, size_context, eye_context> arg_;

        void flush_matrix();
    };

    export class matrix_display_def
    {
    public:
        [[nodiscard]] id_type id() const noexcept;
        [[nodiscard]] text_type name() const noexcept;
        [[nodiscard]] text_type description() const noexcept;

        node_instance_proxy create_node() noexcept;

        node_instance_proxy load(archive::input_archive_t &);
        void save(archive::output_archive_t &, node_instance_proxy &p);
    };

    export class matrix_display_node
    {
    public:
        [[nodiscard]] id_type type_id() const noexcept;
        [[nodiscard]] std::span<const signature_t> signatures() const noexcept;
        [[nodiscard]] std::size_t current_variant() const noexcept;
        bool set_variant(std::size_t) noexcept;
        bool compute(data_sequence) noexcept;
        [[nodiscard]] data_sequence output() const noexcept;
    };
}
