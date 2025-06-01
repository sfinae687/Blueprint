//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-24.
//

module;
#include <proxy/proxy.h>
#include <string>
#include <string_view>

module blueprint.dyn_node;
import :definition;
import :constant_factory;

namespace blueprint::dyn_node::util
{
    using namespace std::string_literals;
    constant_node_definition::constant_node_definition(data_proxy d)
        : constant_node_definition(d, constant_id_base + "." + d->type_id().data())
    {

    }
    constant_node_definition::constant_node_definition(data_proxy d, std::string id)
        : id_(std::move(id)), data_(std::move(d))
    {

    }


    id_type constant_node_definition::id() const noexcept
    {
        return id_;
    }

    /*NOLINT*/ text_type constant_node_definition::name() const noexcept
    {
        return "Constant Node";
    }

    /*NOLINT*/ text_type constant_node_definition::description() const noexcept
    {
        return "The node provide a specilfy result.";
    }

    node_instance_proxy constant_node_definition::create_node()
    {
        return std::make_unique<constant_node_instance>(id_, data_->clone());
    }

    id_type constant_node_instance::type_id() const noexcept
    {
        return id_;
    }

    std::span<const signature_t> constant_node_instance::signatures() const noexcept
    {
        return {&sig_, 1};
    }

    std::size_t constant_node_instance::current_variant() const noexcept
    {
        return 0;
    }

    bool constant_node_instance::set_variant(std::size_t) noexcept
    {
        return false;
    }

    bool constant_node_instance::compute(data_sequence) noexcept
    {
        return false;
    }

    data_sequence constant_node_instance::output() const noexcept
    {
        return {data_};
    }

    constant_node_instance::constant_node_instance(id_type id, data_proxy d)
        : id_(id), data_(std::move(d))
    {
        sig_.input = {};
        sig_.output = {data_->type_id()};
    }

} // namespace blueprint::dyn_node::util
