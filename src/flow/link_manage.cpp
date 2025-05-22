//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

module;
#include <boost/log/common.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <ranges>
#include <utility>

module blueprint.flow;
import :link_manage;
import :debug;
import :NOID;
import blueprint.dyn_node;

namespace blueprint::flow
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    link_manager::link_manager(node_instance_manager& n) : instance_info_(n) {}

    bool link_manager::is_connected(output_t output, input_t input) const noexcept
    {
        auto &&input_index = index_.get<1>();

        auto iter = input_index.find(input);
        if (iter != input_index.end())
        {
            return std::get<2>(*iter) == output;
        }
        return false;

    }

    bool link_manager::have_connection(input_t ch) const noexcept
    {
        auto&& input_index = index_.get<1>();
        return input_index.contains(ch);
    }
    bool link_manager::empty() const noexcept
    {
        return index_.empty();
    }
    std::vector<link_manager::link_t> link_manager::all_link() const noexcept
    {
        namespace ranges = std::ranges;
        namespace views = std::views;

        return ranges::to<std::vector>(index_.get<0>() | views::elements<0>);
        // return index_.get<0>() | views::elements<0> | ranges::to<std::vector>(); // 为什么这个过不了编译
    }

    std::optional<link_manager::link_t> link_manager::create_link(output_t output, input_t input) noexcept
    {
        auto&& input_index = index_.get<1>();

        auto iter = input_index.find(input);
        if (iter != input_index.end())
        {
            if (std::get<2>(*iter) != output)
            {
                return std::nullopt;
            }
            return std::get<0>(*iter);
        }

        auto input_node = node_id(input);
        auto input_ind = channel_index(input);
        auto output_node = node_id(output);
        auto output_ind = channel_index(output);

        auto&& input_isn = instance_info_.get_handler(input_node).node_instance();
        auto&& output_isn = instance_info_.get_handler(output_node).node_instance();

        assert(input_isn);
        assert(output_isn);

        if (!dyn_node::util::passable(output_isn, output_ind, input_isn, input_ind))
        {
            return std::nullopt;
        }
        return do_connect(output, input);
    }

    bool link_manager::remove_link(output_t output, input_t input)
    {
        auto&& input_index = index_.get<1>();

        auto iter = input_index.find(input);
        if (iter != input_index.end() && std::get<2>(*iter) == output)
        {
            return do_remove(index_.project<0>(iter));
        }
        BOOST_LOG_SEV(flow_logger, warning) << "Try erasing the link that has not been existing";
        return false;
    }
    bool link_manager::remove_link(link_t id) noexcept
    {
        auto&& link_index = index_.get<0>();

        auto iter = link_index.find(id);
        if (iter != link_index.end())
        {
            return do_remove(iter);
        }
        BOOST_LOG_SEV(flow_logger, warning) << "Try erasing the link that has not been existing";
        return false;
    }

    bool link_manager::detach_node(no_id nd) noexcept
    {
        remove_node_output(nd);
        remove_node_input(nd);
        return true;
    }

    std::optional<std::pair<link_manager::input_t, link_manager::output_t>> link_manager::query_link(link_t lk) const noexcept
    {
        auto &&link_index = index_.get<0>();

        auto lk_iter = link_index.find(lk);
        if (lk_iter != link_index.end())
        {
            return std::pair{std::get<1>(*lk_iter), std::get<2>(*lk_iter)};
        }
        return std::nullopt;
    }
    std::vector<link_manager::input_t> link_manager::to_input(output_t ot) const noexcept
    {
        namespace ranges = std::ranges;
        std::vector<input_t> rt{};

        auto &&output_index = index_.get<2>();
        auto [l_iter, u_iter] = output_index.equal_range(ot);
        ranges::subrange rg(l_iter, u_iter);

        for (auto [lk, it, ot] : rg)
        {
            rt.push_back(it);
        }

        return rt;
    }
    std::optional<link_manager::output_t> link_manager::to_output(input_t input_id) const noexcept
    {
        auto&& input_index = index_.get<1>();
        auto iter = input_index.find(input_id);

        if (iter == input_index.end())
        {
            return std::nullopt;
        }
        return std::get<2>(*iter);
    }

    std::optional<no_id> link_manager::do_connect(output_t output, input_t input)
    {
        auto new_id = last_id_++;
        auto&& link_index = index_.get<0>();
        link_index.insert({new_id, input, output});
        return new_id;
    }

    bool link_manager::do_remove(link_index_type::iterator iter)
    {
        auto&& link_index = index_.get<0>();
        link_index.erase(iter);
        return true;
    }



    void link_manager::remove_node_input(no_id nd) noexcept
    {
        auto min_input_id = min_input_channel_id(nd);
        auto max_input_id = max_input_channel_id(nd);

        auto &&index = index_.get<1>();
        auto l_iter = index.lower_bound(min_input_id);
        auto u_iter = index.upper_bound(max_input_id);
        index.erase(l_iter, u_iter);
        for (auto c_iter = l_iter; c_iter != u_iter; )
        {
            auto n_iter = ranges::next(c_iter);
            do_remove(index_.project<0>(c_iter));
            c_iter = n_iter;
        }
    }
    void link_manager::remove_node_output(no_id nd) noexcept
    {
        auto min_output_id = min_output_channel_id(nd);
        auto max_output_id = max_output_channel_id(nd);

        auto &&index = index_.get<2>();
        auto l_iter = index.lower_bound(min_output_id);
        auto u_iter = index.upper_bound(max_output_id);
        for (auto c_iter = l_iter; c_iter != u_iter; )
        {
            auto n_iter = ranges::next(c_iter);
            do_remove(index_.project<0>(c_iter));
            c_iter = n_iter;
        }
    }

} // namespace blueprint::flow
