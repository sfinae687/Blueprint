//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//
module;

#include <memory>

#include <string>

module blueprint.builtin_node;
import blueprint.dyn_node;
import :load_image;

namespace blueprint::builtin
{
    using dyn_node::util::constant_node_definition;

    class load_image_definition final : public constant_node_definition
    {
    public:
        using constant_node_definition::constant_node_definition;

        [[nodiscard]] text_type name() const noexcept
        {
            return "Load image";
        }
        [[nodiscard]] text_type description() const noexcept
        {
            return "Load a image from file";
        }
    };

    dyn_node::node_definition_proxy def_load_image_node()
    {
        using dyn_node::util::constant_node_definition;
        using dyn_node::input_sequence_t;
        using dyn_node::output_sequence_t;

        std::string def_id = load_image_node_id;
        return std::make_shared<load_image_definition>(std::string_view(image_id), std::move(def_id));
    }
}