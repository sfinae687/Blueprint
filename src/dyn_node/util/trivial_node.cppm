//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-24.
//

module;
#include <proxy/proxy.h>
#include <vector>

#include <any>

export module blueprint.dyn_node:trivial_node;
import :definition;
import :any_context;

namespace blueprint::dyn_node::util
{

    /// trivial node provides basic information about a node, but it doesn't provide any actually computable service.
    export class trivial_node_definition
    {
        using id_sequence = std::pmr::vector<id_type>;

    public:
        trivial_node_definition(text_type name, text_type description, text_type id, input_sequence_t input,
                                output_sequence_t output);
        trivial_node_definition(const trivial_node_definition&) = delete;
        trivial_node_definition& operator=(const trivial_node_definition&) = delete;

        [[nodiscard]] text_type name() const noexcept;
        [[nodiscard]] text_type description() const noexcept;
        [[nodiscard]] id_type id() const noexcept;

        node_instance_proxy create_node();
        node_instance_proxy load_node(binary_archive &);

    private:
        id_type id_;
        text_type name_;
        text_type description_;
        id_sequence node_input;
        id_sequence node_output;
    };
    static_assert(node_definition<trivial_node_definition>);

    export class trivial_node_instance : public any_context
    {
    public:
        trivial_node_instance(id_type id, input_sequence_t input, output_sequence_t output);
        trivial_node_instance(const trivial_node_instance&) = delete;
        trivial_node_instance& operator=(const trivial_node_instance&) = delete;

        [[nodiscard]] id_type type_id() const noexcept;

        [[nodiscard]] std::span<const signature_t> signatures() const noexcept;
        [[nodiscard]] std::size_t current_variant() const noexcept;
        [[nodiscard]] bool set_variant(std::size_t) const noexcept;
        [[nodiscard("Check the state of compute is always neccessary")]]
        bool compute(data_sequence) noexcept;
        [[nodiscard]] data_sequence output() const noexcept;

        [[nodiscard]] binary_archive serialize() const;


    private:
        id_type id_;
        signature_t sig_;
    };
    static_assert(node_instance<trivial_node_instance>);

}
