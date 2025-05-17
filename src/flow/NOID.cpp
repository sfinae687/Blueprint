//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-17.
//

module;
#include <boost/log/trivial.hpp>

#include <utility>
#include <limits>
#include <climits>

module blueprint.flow;
import :NOID;
import :debug;

namespace blueprint::flow
{

    namespace details
    {
        constexpr no_id bit_at_4 = (no_id{1} << 3);
        constexpr auto no_id_bits = sizeof(no_id) * CHAR_BIT;
        constexpr std::pair<bool, no_id> checked_off_4(no_id x)
        {
            constexpr auto off4_max_base = static_cast<no_id>(1) << (no_id_bits - 4);
            bool is_safe = true;
            if (x >= off4_max_base)
            {
                is_safe = false;
            }
            return {is_safe, x << 4};
        }

        no_id compute_channel(no_id node_id, std::size_t i, bool bit4)
        {
            auto [ff, b] = details::checked_off_4(node_id);
            if (!ff)
            {
                BOOST_LOG_SEV(flow_logger, error) << "Compute a channel id with too large node_id";
            }
            return (b + i) | (bit4 << 3);

        }
    }

    no_id output_channel_id(no_id node_id, std::size_t i)
    {
        return details::compute_channel(node_id, i, true);
    }

    no_id input_channel_id(no_id node_id, std::size_t i) { return details::compute_channel(node_id, i, false); }

    no_id max_input_channel_id(no_id node_id)
    {
        return input_channel_id(node_id, (1 << 3) - 1);
    }
    no_id min_input_channel_id(no_id node_id)
    {
        return input_channel_id(node_id, 0);
    }
    no_id max_output_channel_id(no_id node_id)
    {
        return output_channel_id(node_id, (1 << 3) - 1);
    }
    no_id min_output_channel_id(no_id node_id)
    {
        return output_channel_id(node_id, 0);
    }

    constexpr no_id node_id(no_id channel_id) noexcept { return channel_id >> 4; }

    constexpr channel_type_t channel_type(no_id channel_id) noexcept
    {
        bool is_output = channel_id & details::bit_at_4;
        return is_output ? channel_type_t::OUTPUT : channel_type_t::INPUT;
    }

    constexpr std::size_t channel_index(no_id channel_id) noexcept
    {
        return channel_id & 0x7;
    }

    constexpr no_id no_id_adc(no_id& id) noexcept
    {
        constexpr no_id mask = std::numeric_limits<no_id>::max() >> 4;
        return (id++) & mask;
    }

    constexpr std::tuple<no_id, channel_type_t, std::size_t> channel_id_decompose(no_id id) noexcept
    {
        auto nid = node_id(id);
        auto ct = channel_type(id);
        auto off = channel_index(id);
        return {nid, ct, off};
    }
}