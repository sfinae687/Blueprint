//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-6.
//

module;
#include <boost/mp11.hpp>

#include <optional>
#include <utility>
#include <compare>

export module blueprint.flow:link_events;
import :link_manage;
import :node_instance_manage;
import blueprint.scheduler;

namespace blueprint::flow
{
    // Event

    export enum class link_event_type {
        ADD,
        REMOVE,
    };

    export struct link_event_context
    {
        link_event_type ty;
        no_id input;
        no_id output;
        node_instance_manager * node_manger;
        link_manager * link_manager;

        // default compare
        friend bool operator== (const link_event_context &, const link_event_context &) = default;

    };

    export struct link_event
    {
        using data_type = link_event_context;
    };

    static_assert(std::is_trivially_copy_assignable_v<link_event::data_type>);
    static_assert(std::is_trivially_copy_assignable_v<link_event_context>);
    static_assert(scheduler::event<link_event>);

    // link manager with event emitter

    namespace details
    {
        template <typename T>
        struct is_event_queue_with_link_event_type : std::false_type {};

        template <scheduler::event... ES>
            requires boost::mp11::mp_contains<boost::mp11::mp_list<ES...>, link_event>::value
        struct is_event_queue_with_link_event_type<scheduler::event_queue<ES...>> : std::true_type {};

        template <typename T>
        concept event_queue_with_link_event_type = is_event_queue_with_link_event_type<T>::value;
    }


    export template <details::event_queue_with_link_event_type Q>
    class link_manager_with_event : public link_manager
    {
    public:
        link_manager_with_event(Q& que, node_instance_manager& mg) : link_manager(mg), event_queue(que) {}

    protected:
        std::optional<no_id> do_connect(output_t output, input_t input) override;
        bool do_remove(link_index_type::iterator) override;

    private:
        Q& event_queue;
    };

    template <details::event_queue_with_link_event_type Q>
    std::optional<no_id> link_manager_with_event<Q>::do_connect(output_t output, input_t input)
    {

        event_queue.template push<link_event>({
            link_event_type::ADD, input, output, &instance_info_, this
        });

        return link_manager::do_connect(output, input);
    }
    template <details::event_queue_with_link_event_type Q>
    bool link_manager_with_event<Q>::do_remove(link_index_type::iterator iterator)
    {
        auto [lk, input, output] = *iterator;

        event_queue.template push<link_event>({
            link_event_type::REMOVE, input, output, &instance_info_, this
        });
        return link_manager::do_remove(iterator);
    }
} // namespace blueprint::flow
