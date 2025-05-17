//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-13.
//

module;
#include <unordered_map>

export module blueprint.constraint:constraint_infomation;
import :node_state;
import blueprint.flow;

namespace blueprint::constraint
{

    /**
     * An additional layer to attach node state information to node instances.
     */
    class constraint_information
    {
    public:

        // Copy control: Not copyable and movable.
        constraint_information(const constraint_information& other) = delete;
        constraint_information& operator=(const constraint_information& other) = delete;
        ~constraint_information() = default;

        // constructor

        constraint_information(flow::node_instance_manager &, flow::link_manager &);

        // link operation

        /**
         * Update the node state by link information.
         * @param nd The id of node.
         * @return The node state of the specified node.
         */
        node_state flush_node(flow::no_id nd) noexcept;

        /**
         * Update all state of nodes.
         */
        void flush_all_state() noexcept;

        /**
         * A conventional operation to update node state by a specified link.
         *
         * @pre The specified link has been created.
         *
         * @param output The channel id of output
         * @param input The channel id of input
         */
        void add_link(flow::no_id output, flow::no_id input) noexcept;
        /**
         * A conventional operation to update node state by a specified link.
         *
         * @pre The specified link has been created.
         *
         * @param lk The id of a link.
         */
        void add_link(flow::no_id lk) noexcept;

        /**
         * A conventional operation to update node state by a link.
         *
         * @pre The specified link has been removed.
         *
         * @param output The output channel id
         * @param input The input channel id
         */
        void remove_link(flow::no_id output, flow::no_id input) noexcept;

        // query node state

        node_state query_node() const noexcept;

    private:
        flow::node_instance_manager &nd_;
        flow::link_manager &lk_;

        std::unordered_map<flow::no_id, node_state> node_state_;
    };
}
