//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

module;
#include <boost/log/common.hpp>

#include <utility>

module blueprint.flow;
import :link_manage;
import :debug;
import :NOID;

namespace blueprint::flow
{

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
        auto &&input_index = index_.get<1>();
        return input_index.contains(ch);
    }

    std::optional<link_manager::link_t> link_manager::create_link(output_t output, input_t input)
    {
        auto &&input_index = index_.get<1>();

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

        auto &&input_isn = instance_info_.get_instance(input_node).node_instance();
        auto &&output_isn = instance_info_.get_instance(output_node).node_instance();

        assert(input_isn);
        assert(output_isn);

        if (input_isn->channels()[input_ind] != output_isn->channels()[output_ind])
        {
            return std::nullopt;
        }
        return do_connect(output, input);

    }

    void link_manager::remove_link(output_t output, input_t input)
    {
        auto &&input_index = index_.get<1>();

        auto iter = input_index.find(input);
        if (iter != input_index.end() && std::get<2>(*iter) == output)
        {
            do_remove(index_.project<0>(iter));
        }
        else
        {
            BOOST_LOG_SEV(flow_logger, warning) << "Try erasing the link that has not been existing";
        }
    }


    no_id link_manager::do_connect(output_t output, input_t input)
    {
        auto new_id = last_id_++;
        auto &&link_index = index_.get<0>();
        link_index.insert({new_id, input, output});
        return new_id;
    }

    void link_manager::do_remove(link_index_type::iterator iter)
    {
        auto &&link_index = index_.get<0>();
        link_index.erase(iter);
    }




} // namespace blueprint::flow
