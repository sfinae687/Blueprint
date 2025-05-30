//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-19.
//

/// @file
/// The implementation of constraint_information

module;
#include <boost/log/trivial.hpp>
#include <boost/hof.hpp>
#include <boost/variant2.hpp>

#include <expected>
#include <ranges>

module blueprint.constraint;
import :constraint_infomation;
import :debug;
import blueprint.dyn_node;
import blueprint.flow;

namespace blueprint::constraint
{
    using enum boost::log::trivial::severity_level;
    namespace ranges = std::ranges;
    namespace views = std::views;

    constraint_flow::constraint_flow(flow::node_instance_manager& nd)
        : link_manager(nd)
    {
        BOOST_LOG_SEV(constraint_log, trace) << "Constraint information tracer has initialized";
    }


    bool constraint_flow::flush_node(node_id id) noexcept
    {
        using namespace dyn_node::util;
        using enum node_state;

        // Read the state of the node

        auto status_iter = status_.find(id);
        if (status_iter == status_.end())
        {
            status_iter = status_.insert(status_iter, {id, NOT_PREPARED});
        }

        // Unable to change

        if (status_iter->second == COMPUTING)
        {
            return false;
        }

        // Otherwise, check input connect state


        auto&& node_inst = instance_info_.get_handler(id).node_instance();
        assert(node_inst != nullptr);
        auto&& [input, output] = current_signature(node_inst);
        const auto input_size = input.size();

        const auto min_input_channel = flow::input_channel_id(id, 0);
        const auto max_input_channel = flow::input_channel_id(id, input_size);

        auto&& ct = input_state_counter_[id];
        ct.consistent = ct.inconsistent = ct.undefined = 0;

        for (auto i = min_input_channel; i < max_input_channel; ++i)
        {
            if (input_channel_state(i) == input_link_state_t::UNDEFINED)
            {
                ct.undefined++;
            }
            else if (is_consistent(i))
            {
                ct.consistent++;
            }
            else
            {
                ct.inconsistent++;
            }
        }

        return do_flush_state_by_count(id);
    }

    bool constraint_flow::set_date(input_id in, dyn_node::data_proxy p, bool is_dirty) noexcept
    {
        using namespace boost;
        using enum input_link_state_t;

        // Set new data => Override the old date, Or fill an undefined input channel.
        // Connected input channel is unable to be set, because of no GUI provided.
        // TODO But there is still possibility that set a data by code.

        --input_channel_counter(in);

        const auto current_in_state = input_channel_state(in);
        const auto in_node = flow::node_id(in);
        const auto in_node_state = state(in_node);

        assert(current_in_state == SET || current_in_state == UNDEFINED);


        bool input_consistent;

        // If clean-state is input_set or input_set_changed, it needs to be updated.
        // Otherwise, the input channel is always inconsistent.

        // 1. update clean-state

        auto clean_state = clean_lk_[in];
        visit(hof::first_of(
            [&](variant2::variant<input_set, input_set_changed>)
            {
                if (is_dirty)
                {
                    clean_lk_[in] = input_set_changed{};
                } else
                {
                    clean_lk_[in] = input_set{};
                }
                input_consistent = is_dirty;
            },
            [&] (auto &&)
            {
                input_consistent = false;
            }
        ), clean_state);

        // 2. update set-date

        set_date_[in] = std::move(p);

        // 3. update channel-state-counter and channel state

        auto &&st = input_state_counter_[in_node];
        if (input_consistent)
        {
            st.consistent++;
        }
        else
        {
            st.inconsistent++;
        }

        // 4. update node state

        if (do_flush_state_by_count(in_node))
        {
            update_from_node(in_node);
        }

        return true;

    }


    bool constraint_flow::unset_date(input_id in) noexcept
    {
        using enum node_state;
        if (!is_updatable(state(flow::node_id(in))))
        {
            return false;
        }

        --input_channel_counter(in);

        set_date_.erase(in);

        // update input channel state

        ++input_channel_counter(in); // Should increase the undefined state.

        if (do_flush_state_by_count(in))
        {
            update_from_node(flow::node_id(in));
        }

        return true;
    }

    constraint_flow::seek_data_result constraint_flow::seek_data(flow::no_id id) const noexcept
    {
        switch (flow::channel_type(id))
        {
        case flow::channel_type_t::INPUT:
            return seek_input(id);
        case flow::channel_type_t::OUTPUT:
            return seek_output(id);
        }
        return std::unexpected(seek_data_error::undefined);
    }

    constraint_flow::seek_data_result constraint_flow::seek_input(input_id in) const noexcept
    {
        using enum seek_data_error;
        using enum input_link_state_t;
        auto input_state = input_channel_state(in);

        if (flow::channel_type(in) != flow::channel_type_t::INPUT)
        {
            return std::unexpected(unexpected_id_type);
        }

        if (input_state == UNDEFINED)
        {
            return std::unexpected(undefined);
        }

        if (auto set_iter = set_date_.find(in); set_iter != set_date_.end())
        {
            return set_iter->second;
        }

        return seek_output(to_output(in).value());
    }
    constraint_flow::seek_data_result constraint_flow::seek_output(output_id out) const noexcept
    {
        using enum seek_data_error;
        using std::unexpected;

        if (flow::channel_type(out) != flow::channel_type_t::OUTPUT)
        {
            return unexpected(unexpected_id_type);
        }

        auto output_node = flow::node_id(out);
        auto output_ind = flow::channel_index(out);
        auto &&inst = instance_info_.get_handler(output_node).node_instance();

        auto &&outputs = inst->output();

        if (output_ind >= outputs.size())
        {
            return unexpected(undefined);
        }

        return outputs[output_ind];
    }
    std::expected<dyn_node::data_sequence, constraint_flow::gather_input_error>
    constraint_flow::gather_input(flow::no_id id) const noexcept
    {
        auto nd = flow::node_id(id);
        auto&& nd_inst = instance_info_.get_handler(id).node_instance();
        const auto sig = dyn_node::util::current_signature(nd_inst);
        auto input_size = sig.input.size();

        dyn_node::data_sequence seq{};
        for (std::size_t i = 0; i < input_size; ++i)
        {
            auto dd = seek_input(flow::input_channel_id(id, i));
            if (dd)
            {
                seq.emplace_back(std::move(dd.value()));
            }
            else
            {
                return std::unexpected(gather_input_error::undefined_input);
            }
        }

        return seq;
    }

    node_state& constraint_flow::state(flow::no_id id) noexcept
    {
        auto st_iter = status_.find(id);
        if (st_iter == status_.end())
        {
            flush_node(id);
            st_iter = status_.find(id);
        }
        return st_iter->second;
    }

    bool constraint_flow::mark_computing(flow::no_id id) noexcept
    {
        if (state(id) == node_state::NOT_PREPARED || state(id) == node_state::COMPUTING)
        {
            return false;
        }
        if (state(id) == node_state::CLEAN)
        {
            BOOST_LOG_SEV(constraint_log, warning) << "Recalculate a clean node";
        }

        auto &&nd_inst = instance_info_.get_handler(id).node_instance();
        const auto sig = dyn_node::util::current_signature(nd_inst);
        auto input_size = sig.input.size();

        for (std::size_t i=0; i<input_size; ++i)
        {
            using enum input_link_state_t;

            auto in_id = flow::input_channel_id(id, i);
            auto ins = input_channel_state(in_id);
            assert(ins != UNDEFINED);

            switch (ins)
            {
            case SET:
                clean_lk_[in_id] = input_set{};
                break;
            case CONNECTED:
                clean_lk_[in_id] = input_connected{*to_output(in_id), revision(id)};
                break;
            default:
                __builtin_unreachable();
            }
        }

        status_[id] = node_state::COMPUTING;
        return true;
    }

    bool constraint_flow::mark_clean(flow::no_id id) noexcept
    {
        using enum node_state;
        if (state(id) != COMPUTING)
        {
            return false;
        }
        status_[id] = CLEAN;
        clean_rv_[id]++;
        update_from_node(id);
        return true;
    }

    bool constraint_flow::mark_dirty(flow::no_id id) noexcept
    {
        using enum node_state;
        if (state(id) != CLEAN)
        {
            return false;
        }
        status_[id] = DIRTY;
        update_from_node(id);
        return true;
    }

    bool constraint_flow::mark_error(flow::no_id id) noexcept
    {
        using enum node_state;
        if (state(id) != COMPUTING)
        {
            return false;
        }
        status_[id] = ERROR;
        update_from_node(id);
        return true;
    }

    std::vector<constraint_flow::node_id> constraint_flow::dump_ready() noexcept
    {
        auto ans = status_ | views::keys | views::filter([&](node_id id)
        {
            return is_ready(id);
        });

        return ranges::to<std::vector>(ans);
    }

    std::optional<unsigned long> constraint_flow::do_connect(output_t output, input_t input)
    {
        using enum node_state;

        const auto in_node = flow::node_id(input);

        const auto in_node_state = state(in_node);

        if (! is_updatable(in_node_state))
        {
            return std::nullopt;
        }

        return do_trace_link(output, input);
    }

    bool constraint_flow::do_remove(link_index_type::iterator iterator)
    {
        auto [lk, in, out] = *iterator;

        const auto in_node = flow::node_id(in);
        if (!is_updatable(state(in_node)))
        {
            return false;
        }

        return do_drop_link(iterator);
    }

    bool constraint_flow::do_detach(flow::no_id id) noexcept
    {
        status_.erase(id);
        clean_rv_.erase(id);
        input_state_counter_.erase(id);

        auto min_input_id = flow::min_input_channel_id(id);
        auto max_input_id = flow::max_input_channel_id(id);

        set_date_.erase(set_date_.lower_bound(min_input_id), set_date_.lower_bound(max_input_id));
        clean_lk_.erase(clean_lk_.lower_bound(min_input_id), clean_lk_.lower_bound(max_input_id));

        return true;
    }

    std::size_t& constraint_flow::revision(node_id id) noexcept
    {
        auto rv_iter = clean_rv_.find(id);
        if (rv_iter == clean_rv_.end())
        {
            rv_iter = clean_rv_.insert(rv_iter, {id, -1});
        }
        return rv_iter->second;
    }
    std::optional<constraint_flow::input_id> constraint_flow::do_trace_link(output_id out, input_id in) noexcept
    {
        const auto in_node = flow::node_id(in);

        --input_channel_counter(in);


        // unset data if set-date exists.

        auto set_iter = set_date_.find(in);
        if (set_iter != set_date_.end())
        {
            set_date_.erase(set_iter);
        }

        // create the link

        const auto new_link = link_manager::do_connect(out, in);
        if (! new_link)
        {
            return std::nullopt;
        }

        // update connect state

        ++input_channel_counter(in);

        if (do_flush_state_by_count(in_node))
        {
            update_from_node(flow::node_id(in));
        }

        return new_link;
    }

    bool constraint_flow::do_drop_link(link_index_type::iterator lk) noexcept
    {
        auto [lk_id, in, out] = *lk;

        --input_channel_counter(in);
        flow::link_manager::do_remove(lk);
        ++input_channel_counter(in);


        auto in_node = flow::node_id(in);
        if (do_flush_state_by_count(in_node))
        {
            update_from_node(in_node);
        }

        return true;

    }

    /*NOLINT*/ bool constraint_flow::do_input_state_changed(input_id in) noexcept
    {

        // TODO prevent the recursive call from infinite loop.

        const auto in_node = flow::node_id(in);

        assert(is_updatable(state(in_node)));

        auto&& clean_state = clean_lk_[in];

        const auto current_consistent = is_consistent(in);

        auto ct_iter = input_state_counter_.find(in_node);
        if (ct_iter == input_state_counter_.end())
        {
            flush_node(in_node);
            update_from_node(in_node);
            return true;
        }

        auto&& ct = ct_iter->second;
        if (current_consistent)
        {
            ct.inconsistent--;
            ct.consistent++;
        }
        else
        {
            ct.inconsistent++;
            ct.consistent--;
        }

        if constexpr (constraint_debug)
        {
            const auto ct_sum = ct.inconsistent + ct.consistent + ct.undefined;
            auto&& inst = instance_info_.get_handler(in_node).node_instance();
            const auto input_size = dyn_node::util::current_signature(inst).input.size();
            assert(input_size == ct_sum);
        }

        if (do_flush_state_by_count(in_node))
        {
            update_from_node(in_node);
        }
        return true;
    }

    bool constraint_flow::do_flush_state_by_count(node_id id) noexcept
    {
        using enum node_state;

        auto&& ct = input_state_counter_[id];
        auto&& st = status_[id];
        const auto raw_st = st;

        if (ct.inconsistent == 0 && ct.consistent == 0 && ct.undefined == 0)
        {
            if (raw_st != CLEAN)
            {
                st = DIRTY;
            }
        }
        else if (ct.undefined != 0)
        {
            st = NOT_PREPARED;
        }
        else if (ct.inconsistent != 0)
        {
            st = DIRTY;
        }
        else
        {
            st = CLEAN;
        }

        return raw_st != st;
    }
    /*NOLINT*/ void constraint_flow::update_from_node(node_id nd) noexcept
    {
        auto&& inst = instance_info_.get_handler(nd).node_instance();
        const auto output_size = dyn_node::util::current_signature(inst).output.size();
        for (std::size_t i = 0; i < output_size; ++i)
        {
            const auto next_channel = flow::output_channel_id(nd, i);
            /*NOLINT*/ auto&& next_link = to_input(next_channel);
            for (const auto next_in : next_link)
            {
                do_input_state_changed(next_in);
            }
        }

    }

    bool constraint_flow::is_consistent(input_id in) noexcept
    {
        namespace hof = boost::hof;

        const auto current_state = input_channel_state(in);
        const auto clean_state_iter = clean_lk_.find(in);

        if (clean_state_iter == clean_lk_.end())
        {
            return false;
        }

        auto&& clean_state = clean_state_iter->second;

        return visit<bool>(hof::match([&](input_undefined) constexpr
                                      { return current_state == input_link_state_t::UNDEFINED; },
                                      [&](input_set) { return current_state == input_link_state_t::SET; },
                                      [&](input_set_changed) { return false; },
                                      [&](input_connected ic)
                                      {
                                          if (current_state != input_link_state_t::CONNECTED)
                                          {
                                              return false;
                                          }
                                          auto current_link = to_output(in);
                                          assert(current_link.has_value());
                                          const auto output_state = state(flow::node_id(*current_link));

                                          return *current_link == ic.output_id && output_state == node_state::CLEAN &&
                                              revision(ic.output_id) == ic.clean_revision;
                                      }),
                           clean_state);
    }

    bool constraint_flow::is_ready(node_id id) noexcept
    {
        using enum node_state;
        if (state(id) != DIRTY)
        {
            return false;
        }

        auto hd = instance_info_.get_handler(id);
        assert(hd != nullptr);
        auto &&inst = hd.node_instance();
        auto sig = dyn_node::util::current_signature(inst);
        auto input_size = sig.input.size();

        bool ready_flag = true;
        for (int i=0; i<input_size; ++i)
        {
            auto input_channel_id = flow::input_channel_id(id, i);
            switch (input_channel_state(input_channel_id))
            {

            case input_link_state_t::CONNECTED:
            {
                auto output_channel_id = to_output(input_channel_id);
                assert(output_channel_id);
                auto output_node = flow::node_id(*output_channel_id);
                if (state(output_node) != CLEAN)
                {
                    ready_flag = false;
                    goto CHECK_END;
                }
                break;
            }
            default:
                break;
            }
        }
        CHECK_END:

        return ready_flag;

    }

    auto constraint_flow::input_channel_state(input_id in) const noexcept -> input_link_state_t
    {
        const auto is_set = set_date_.contains(in);
        const auto is_linked = have_connection(in);

        assert(!is_set || !is_linked);

        if (is_linked)
        {
            return input_link_state_t::CONNECTED;
        }
        if (is_set)
        {
            return input_link_state_t::SET;
        }
        return input_link_state_t::UNDEFINED;
    }

    std::size_t& constraint_flow::input_channel_counter(input_id in) noexcept
    {
        using enum input_link_state_t;

        const auto in_node = flow::node_id(in);
        auto st_iter = input_state_counter_.find(in_node);
        if (st_iter == input_state_counter_.end())
        {
            flush_node(in_node);
            st_iter = input_state_counter_.find(in_node);
        }
        auto &&st = st_iter->second;
        if (input_channel_state(in) == UNDEFINED)
        {
            return st.undefined;
        }
        if (is_consistent(in))
        {
            return st.consistent;
        }
        return st.inconsistent;
    }

} // namespace blueprint::constraint
