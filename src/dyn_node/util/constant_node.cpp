//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-24.
//

module;
#include <proxy/proxy.h>

#include <string>
#include <string_view>
#include <iostream>

module blueprint.dyn_node;
import :definition;
import :constant_factory;

namespace blueprint::dyn_node::util
{
    using namespace std::string_literals;

    std::string make_constant_id(std::string_view id)
    {
        return constant_id_base + id.data();
    }

    // definition

    constant_node_definition::constant_node_definition(data_proxy d)
        : constant_node_definition(d, make_constant_id(d->type_id()))
    {

    }
    constant_node_definition::constant_node_definition(data_proxy d, std::string id) :
        id_(std::move(id)), data_(std::move(d)), sig_({{}, {data_->type_id()}})
    {
    }

    constant_node_definition::constant_node_definition(id_type output_ty)
        : id_(make_constant_id(output_ty)), data_(nullptr), sig_({{}, {output_ty}})
    {

    }
     constant_node_definition::constant_node_definition(id_type output_ty, std::string def_id)
         : id_(std::move(def_id)), data_(nullptr), sig_({}, {output_ty})
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
        return "The node provide a specify result.";
    }

    node_instance_proxy constant_node_definition::create_node()
    {
        return std::make_unique<constant_node_instance>(id_, data_ ? data_->clone() : nullptr, *this);
    }

    // Instance

    id_type constant_node_instance::type_id() const noexcept
    {
        return id_;
    }

    std::span<const signature_t> constant_node_instance::signatures() const noexcept
    {
        return {&def_.sig_, 1};
    }

    std::size_t constant_node_instance::current_variant() const noexcept
    {
        return 0;
    }

    bool constant_node_instance::set_variant(std::size_t) noexcept
    {
        return false;
    }

    bool constant_node_instance::compute(data_sequence ds) noexcept
    {
        return ds.size() == 0 && data_.has_value();
    }

    data_sequence constant_node_instance::output() const noexcept
    {
        return {data_};
    }
    void constant_node_instance::set_output(data_proxy d) noexcept
    {
        assert(d->type_id() == def_.sig_.output[0]);
        data_ = std::move(d);
        assert(data_->type_id() == def_.sig_.output[0]);
    }

    constant_node_instance::constant_node_instance(id_type id, data_proxy d, constant_node_definition &def)
        : id_(id), data_(std::move(d)), def_(def)
    {}

} // namespace blueprint::dyn_node::util
