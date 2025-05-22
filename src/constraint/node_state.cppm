//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-13.
//

module;
#include <cassert>

export module blueprint.constraint:node_state;

namespace blueprint::constraint
{

    /**
     * Descript the state of the relationship between the input and output of a node.
     *
     * @todo The current model could only mark whether a complete node is dirty. A model with channel-diameter mark should be more suitable
     */
    export enum class node_state
    {
        // Public state
        NOT_PREPARED, ///< The link state of the node is not prepared to produce a result.

        DIRTY, ///< The link state of the node is different from the last result computation.
        COMPUTING, ///< The node is in computing. Can't change the link state of it.
        CLEAN, ///< The link state of the node is consistent with the result.

        ERROR, ///< An error occurs during computing.
        UNKNOWN,
    };

    /**
     * Transform an inner node state to public one.
     * @param s The node state that may be inner state
     * @return The public state
     */
    export constexpr node_state node_state_classify(node_state s)
    {
        switch (s)
        {
        case node_state::NOT_PREPARED:
        case node_state::DIRTY:
        case node_state::CLEAN:
        case node_state::COMPUTING:
        case node_state::ERROR:
            return s;

        default:
            return node_state::UNKNOWN;
        }
    }

    export constexpr bool is_updatable(node_state s)
    {
        return s != node_state::COMPUTING;
    }

}
