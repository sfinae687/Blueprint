//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

module;
#include <proxy/proxy.h>

#include <boost/log/common.hpp>

#include <shared_mutex>
#include <unordered_map>
#include <memory>
#include <vector>

module blueprint.flow;
import blueprint.dyn_node;
import :node_instance_manage;
import :NOID;
import :debug;

namespace blueprint::flow
{
    no_id node_instance_handler::node_id() const noexcept {
        return it_->second.second;
    }

    const dyn_node::node_instance_proxy& node_instance_handler::node_instance() const noexcept {
        return it_->second.first;
    }

    dyn_node::node_instance_proxy& node_instance_handler::node_instance() noexcept
    {
        return it_->second.first;
    }


    void node_instance_handler::remove()
    {
        std::lock_guard gd(manager_->lock_);
        manager_->list_.erase(it_);
        manager_ = nullptr;
        it_ = {};
    }

    node_instance_handler node_instance_manager::add_instance(dyn_node::node_instance_proxy p)
    {
        std::lock_guard gd(lock_);
        auto current_id = no_id_adc(last_id_);
        auto rt = list_.insert({current_id, {std::move(p), current_id}}).first;
        return {*this, rt};
    }

    node_instance_handler node_instance_manager::add_instance(dyn_node::node_instance_proxy p, no_id the_id)
    {
        auto [id_iter, sc] = list_.insert({the_id, {std::move(p), the_id}});
        if (! sc)
        {
            BOOST_LOG_SEV(flow_logger, error) << "The instance addition is prevent by the existed id";
            return nullptr;
        }

        last_id_ = std::max(last_id_, the_id + 1);

        return {*this, id_iter};
    }


    node_instance_handler node_instance_manager::get_handler(no_id id) noexcept
    {
        std::shared_lock gd(lock_);
        auto iter = list_.find(id);
        if (iter == list_.end())
        {
            return nullptr;
        }
        return {*this, iter};
    }
    std::vector<node_instance_handler> node_instance_manager::dump_handler() noexcept
    {
        std::shared_lock gd(lock_);

        std::vector<node_instance_handler> rt{};
        for (auto iter = list_.begin(); iter != list_.end(); ++iter)
        {
            rt.push_back({*this, iter});
        }
        return rt;
    }


} // namespace blueprint::flow
