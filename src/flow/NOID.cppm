//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

module;
#include <utility>

export module blueprint.flow:NOID;
import :debug;

namespace blueprint::flow
{
    export using no_id = std::size_t;

    export enum class channel_type_t {
        INPUT,
        OUTPUT,
    };

    /**
     * Compute the output id based on node id and the index of channels. Note, computing is based on the bit
     * operation. It is hardly to grantee that the id is unique when the id is too large.
     */
    export no_id output_channel_id(no_id node_id, std::size_t i);

    /**
     * Compute the output id based on node id and the index of channels. Note, computing is based on the bit
     * operation. It is hardly to grantee that the id is unique when the id is too large.
     */
    export no_id input_channel_id(no_id node_id, std::size_t i);

    export no_id max_input_channel_id(no_id node_id);
    export no_id min_input_channel_id(no_id node_id);
    export no_id max_output_channel_id(no_id node_id);
    export no_id min_output_channel_id(no_id node_id);

    /**
     * Get node from channel id.
     * @param channel_id The id of a input or output channel.
     * @return The Node id.
     */
    export constexpr no_id node_id(no_id channel_id) noexcept;

    /**
     * Get input or output type of channel.
     * @param channel_id The id of input channel or output channel.
     * @return The type of given channel.
     */
    export constexpr channel_type_t channel_type(no_id channel_id) noexcept;

    /**
     * Get channel index from channel id.
     * @param channel_id The id of channel
     * @return The index among the node.
     */
    export constexpr std::size_t channel_index(no_id channel_id) noexcept;

    export constexpr std::tuple<no_id, channel_type_t, std::size_t> channel_id_decompose(no_id) noexcept;

    export constexpr std::size_t no_id_adc(no_id &id) noexcept;

}
