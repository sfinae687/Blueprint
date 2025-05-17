//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-17.
//

module;
#include <proxy.h>

#include <span>
#include <utility>
#include <memory>

module blueprint.dyn_node;
import :utility;

namespace blueprint::dyn_node::util
{
    // trivial_node_definition

    trivial_node_definition::trivial_node_definition(text_type name, text_type description, text_type id,
                                                     id_sequence input, id_sequence output) :
        name_(name), description_(description), id_(id), node_input(std::move(input)), node_output(std::move(output))
    {
    }
    text_type trivial_node_definition::name() const noexcept { return name_; }
    text_type trivial_node_definition::description() const noexcept { return description_; }
    id_type trivial_node_definition::id() const noexcept { return id_; }
    node_instance_proxy trivial_node_definition::create_node()
    {
        auto ptr = std::make_unique<trivial_node_instance>(id_, node_input, node_output);
        return ptr;
    }

    trivial_node_instance::trivial_node_instance(id_type id, input_sequence_t input, output_sequence_t output) :
        id_(id), sig_({std::move(input), std::move(output)})
    {
    }
    id_type trivial_node_instance::type_id() const noexcept { return id_; }
    std::size_t trivial_node_instance::current_variant() const noexcept { return 0; }
    bool trivial_node_instance::set_variant(std::size_t) const noexcept { return false; }
    bool trivial_node_instance::compute(data_sequence) noexcept
    {
        return false;
    }
    data_sequence trivial_node_instance::output() const noexcept
    {
        return {};
    }



} // namespace blueprint::dyn_node::util
