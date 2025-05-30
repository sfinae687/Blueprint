//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-13.
//

module;
#include <boost/variant2.hpp>
#include <boost/hana.hpp>

#include <expected>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

export module blueprint.constraint:constraint_infomation;
import :node_state;
import blueprint.flow;
import blueprint.dyn_node;

namespace blueprint::constraint
{

    namespace details
    {
        using boost::hana::map;
        using boost::hana::pair;
        using boost::hana::type;

        template <typename... TS>
        using counter_type = map<pair<type<TS>, std::size_t>...>;
    }


    /**
     * An additional layer to attach node state and channel information to node instances.
     *
     * ## Update policy
     *
     * The constraint_information maintains the connection state at the last recalculation.
     * If the current input channel is matched with the last updated state, we consider the current state is consistent.
     * If all input channels of a node are consistent, we consider the current result of the node is fresh, and doesn't
     * need to update.
     * Otherwise, we consider the node is dirty and the node would be recalculated soon.
     *
     * The external could set an input channel a date, or connect the input channel with an output channel.
     * If the input channel is set with a date, the external have also the responsibility to determine whether the input
     * channel is consistent.
     * The constraint_information caches the last date set by the external,
     * but it provides little help for the external.
     * If the input channel is connected with an output channel, the constraint_information consider the input channel
     * is consistent when the output node is clean.
     *
     * If the state of a node is changed, it will case the further state update.
     * All nodes that have dependency to the node will be updated at the same time.
     *
     */
    export class constraint_flow final : public flow::link_manager
    {
        using node_id = flow::no_id;
        using input_id = flow::no_id;
        using output_id = flow::no_id;
        using link_id = flow::no_id;

    public:

        // Copy control: Not copyable and movable.

        constraint_flow(const constraint_flow& other) = delete;
        constraint_flow& operator=(const constraint_flow& other) = delete;
        ~constraint_flow() override = default;

        // constructor

        explicit constraint_flow(flow::node_instance_manager&);

        /**
         * Flush the state of given node according the information in link manager.
         *
         * The COMPUTING state will keep.
         *
         * @pre The state of it's input channel and more prior node is updated.
         * @post The state of the given node and all its successor would be updated.
         *
         * @return True if state changes. Otherwise, return false. The node stays in COMPUTING will cause
         * retrace failure.
         */
        bool flush_node(node_id) noexcept;

        ///@}

        // set-date

        bool set_date(input_id, dyn_node::data_proxy p, bool is_dirty = true) noexcept;

        bool unset_date(input_id) noexcept;

        // channel date query

        enum class seek_data_error
        {
            undefined,
            unexpected_id_type,
        };
        using seek_data_result = std::expected<dyn_node::data_proxy, seek_data_error>;

        /// Get the output date if the id is belonged to an output channel, otherwise the id is an input channel id.
        /// In this case,
        /// get the data of output channel the input channel is connected to, or the set date for the
        /// input channel.

        seek_data_result seek_data(flow::no_id) const noexcept;

        seek_data_result seek_input(input_id) const noexcept;

        seek_data_result seek_output(output_id) const noexcept;


        /**
         * The enum to indicate the reason for the gather_input fail.
         *
         * @see gather_input
         */
        enum class gather_input_error
        {
            undefined_input,
            dirty_dependence,
        };

        /**
         * Collect input date for the specified node.
         *
         * @pre The all output channel connected to the given node is cleanly computed.
         *
         * @return A sequence of input data for given node
         */
        std::expected<dyn_node::data_sequence, gather_input_error>
        gather_input(flow::no_id) const noexcept;

        // node state

        /// The state of the given node
        node_state& state(flow::no_id) noexcept;

        bool mark_computing(flow::no_id) noexcept;
        bool mark_clean(flow::no_id) noexcept;
        bool mark_dirty(flow::no_id) noexcept;
        bool mark_error(flow::no_id) noexcept;

        std::vector<node_id> dump_ready() noexcept;

    protected:

        std::optional<unsigned long> do_connect(output_t output, input_t input) override;

        bool do_remove(link_index_type::iterator) override;

        bool do_detach(flow::no_id) noexcept override;

    private:
        std::size_t& revision(node_id id) noexcept;

        /**
         * Trace the new link information without further check.
         *
         * @pre the link between input and output has existed in link manage.
         *
         * @post Link constraint updated.
         * If the current constraint doesn't allow the link, it will be removed from the link manager.
         *
         * @return True if success.
         */
        std::optional<input_id> do_trace_link(output_id, input_id) noexcept;

        bool do_drop_link(link_index_type::iterator lk) noexcept;

        /**
         * Update node state of input side.
         *
         * @pre The consistency of input channel has changed.
         *
         * @todo It's need a kind of delay update to provide prefect update.
         *
         * @return True if success.
         */
        bool do_input_state_changed(input_id) noexcept;

        /**
         * Update the state of given node only according the state counter.
         *
         * @return True if state changes.
         */
        bool do_flush_state_by_count(node_id) noexcept;

        void update_from_node(node_id) noexcept;


        /**
         * Check whether the current data of the specify input channel is same with the last compute
         *
         * It's used for determine whether a node needs to update.
         *
         * @return True if the channel is consistent with last clean result.
         */
        bool is_consistent(input_id) noexcept;

        bool is_ready(node_id) noexcept;

        enum class input_link_state_t
        {
            UNDEFINED,
            SET,
            CONNECTED,
            AMBIGUOUS,
        };

        /**
         * Get the state of input channel.
         * @return the state of given input channel.
         */
        input_link_state_t input_channel_state(input_id) const noexcept;

        std::size_t& input_channel_counter(input_id) noexcept;

        // basic link

        std::unordered_map<node_id, node_state> status_{};
        std::unordered_map<node_id, std::size_t> clean_rv_{};
        std::map<input_id, dyn_node::data_proxy> set_date_{};

        // state trace

        struct input_undefined { };
        struct input_set { };
        struct input_set_changed { };
        struct input_connected
        {
            output_id output_id;
            std::size_t clean_revision;
        };

        using clean_state = boost::variant2::variant<
            input_undefined,
            input_set,
            input_set_changed,
            input_connected
        >;
        /// record the clean link state for the current result.
        std::map<input_id, clean_state> clean_lk_{};

        struct input_state_counter_t
        {
            std::size_t consistent;
            std::size_t inconsistent;
            std::size_t undefined;
        };

        /// record the number of every input channel link state.
        std::unordered_map<node_id, input_state_counter_t> input_state_counter_;
    };
} // namespace blueprint::constraint
