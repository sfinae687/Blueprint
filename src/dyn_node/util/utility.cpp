//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-17.
//

module;
#include <proxy/proxy.h>
#include <boost/hof/unpack.hpp>

#include <span>
#include <utility>
#include <memory>
#include <ranges>
#include <algorithm>
#include <iostream>
#include <format>

module blueprint.dyn_node;
import :utility;

namespace blueprint::dyn_node::util
{
    namespace ranges = std::ranges;
    namespace views = std::views;
    namespace hof = boost::hof;

    // primitive

    bool
    passable(const node_instance_proxy &from, std::size_t fi, const node_instance_proxy &to, std::size_t ti) noexcept
    {
        return current_signature(from).output.at(fi) == current_signature(to).input.at(ti);
    }

    bool
    passable(const data_proxy &from, const node_instance_proxy &to, std::size_t ti) noexcept
    {
        return from->type_id() == current_signature(to).input.at(ti);
    }

    const signature_t& current_signature(const node_instance_proxy &nd) noexcept
    {
        auto ind = nd->current_variant();
        auto &&sigs = nd->signatures();
        return sigs[ind];
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
