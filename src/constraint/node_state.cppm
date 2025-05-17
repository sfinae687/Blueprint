//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-13.
//

export module blueprint.constraint:node_state;

namespace blueprint::constraint
{
    /**
     * Descript the state of the relationship between the input and output of a node.
     */
    enum class node_state
    {
        NOT_PREPARED, ///< Current input config is unable to produce a result.
        DIRTY, ///< Current input could produce an output, but current output is not matched the inputs.
        COMPUTING, ///< The node is processing.
        READY, ///< The computation is finished and the output is matched to input.
    };



}
