//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//

module;

#include <proxy/proxy.h>

#include <opencv2/core.hpp>

#include <filesystem>

export module blueprint.builtin_node:load_image;
import blueprint.dyn_node;

namespace blueprint::builtin
{
    using dyn_node::util::constant_node_definition;
    using dyn_node::text_type;
    using dyn_node::id_type;
    using dyn_node::node_instance_proxy;
    using dyn_node::signature_t;
    using dyn_node::data_sequence;
    namespace fs = std::filesystem;

    export constexpr char load_image_node_id[] = "core.load-image";
    export constexpr char load_image_node_name[] = "Load image";
    export constexpr char load_image_node_description[] = "Load a image form file";

    export class load_image_instance;
    export class load_image_definition;

    class load_image_instance
    {
        friend class load_image_definition;
    public:

        explicit load_image_instance();

        [[nodiscard]] id_type type_id() const noexcept;
        [[nodiscard]] std::span<const signature_t> signatures() const noexcept;
        [[nodiscard]] std::size_t current_variant() const noexcept;
        bool set_variant(std::size_t) noexcept;
        bool compute(data_sequence) noexcept;
        [[nodiscard]] data_sequence output() const noexcept;

        [[nodiscard]] bool has_image() const;

        void set_path(fs::path);
        fs::path path();

    private:
        std::shared_ptr<cv::Mat> img_;
        fs::path path_;
    };

    class load_image_definition
    {
    public:
        [[nodiscard]] id_type id() const noexcept;
        [[nodiscard]] text_type name() const noexcept;
        [[nodiscard]] text_type description() const noexcept;

        node_instance_proxy create_node() noexcept;

        node_instance_proxy load(archive::input_archive_t &);
        void save(archive::output_archive_t &, node_instance_proxy &p);

    };

    static_assert(dyn_node::node_instance<load_image_instance>);
    static_assert(dyn_node::node_definition<load_image_definition>);

    export dyn_node::node_definition_proxy def_load_image_node();
}
