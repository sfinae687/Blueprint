//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-17.
//

module;
#include <proxy.h>
#include <boost/hof/unpack.hpp>

#include <span>
#include <utility>
#include <memory>
#include <ranges>
#include <algorithm>

module blueprint.dyn_node;
import :utility;

namespace blueprint::dyn_node::util
{
    namespace ranges = std::ranges;
    namespace views = std::views;
    namespace hof = boost::hof;

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

    // primitive

    bool
    passable(const node_instance_proxy &from, std::size_t fi, const node_instance_proxy &to, std::size_t ti) noexcept
    {
        return current_signature(from).second.at(fi) == current_signature(to).first.at(ti);
    }

    bool
    passable(const data_proxy &from, const node_instance_proxy &to, std::size_t ti) noexcept
    {
        return from->type_id() == current_signature(to).first.at(ti);
    }

    const signature_t& current_signature(const node_instance_proxy &nd) noexcept
    {
        return nd->signatures()[nd->current_variant()];
    }

    bool node_invoke(node_instance_proxy &nd, data_sequence seq) noexcept
    {
        namespace hof = boost::hof;
        auto [fst, snd] = current_signature(nd);
        auto &&input_sig = fst;
        if (seq.size() != input_sig.size())
        {
            return false;
        }

        auto arg_ty = seq | views::transform([] (data_proxy &p)
        {
            return p->type_id();
        });

        if (ranges::any_of(views::zip(input_sig, arg_ty), hof::unpack(ranges::equal_to{})))
        {
            return false;
        }

        return nd->compute(std::move(seq));

    }

} // namespace blueprint::dyn_node::util
