//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-14.
//

module;
#include <proxy.h>

#include <ranges>
#include <span>
#include <utility>
#include <vector>

export module blueprint.dyn_node:utility;
import :definition;

namespace blueprint::dyn_node::util
{

    // trivial node

    export class trivial_node_definition
    {
        using id_sequence = std::pmr::vector<id_type>;

    public:
        trivial_node_definition(text_type name, text_type description, text_type id, id_sequence input,
                                id_sequence output);
        trivial_node_definition(const trivial_node_definition&) = delete;
        trivial_node_definition& operator=(const trivial_node_definition&) = delete;

        text_type name() const noexcept;
        text_type description() const noexcept;
        id_type id() const noexcept;

        node_instance_proxy create_node();

    private:
        id_type id_;
        text_type name_;
        text_type description_;
        id_sequence node_input;
        id_sequence node_output;
    };
    static_assert(pro::proxiable<std::unique_ptr<trivial_node_definition>, node_definition_facade>);

    export class trivial_node_instance
    {
    public:
        trivial_node_instance(id_type id, input_sequence_t input, output_sequence_t output);
        trivial_node_instance(const trivial_node_instance&) = delete;
        trivial_node_instance& operator=(const trivial_node_instance&) = delete;

        id_type type_id() const noexcept;

        [[nodiscard]] std::span<const signature_t> signatures() const noexcept;
        [[nodiscard]] std::size_t current_variant() const noexcept;
        [[nodiscard]] bool set_variant(std::size_t) const noexcept;
        bool compute(data_sequence) noexcept;
        [[nodiscard]] data_sequence output() const noexcept;


    private:
        id_type id_;
        signature_t sig_;
    };
    static_assert(pro::proxiable<std::unique_ptr<trivial_node_instance>, node_instance_facade>);



} // namespace blueprint::dyn_node::util
