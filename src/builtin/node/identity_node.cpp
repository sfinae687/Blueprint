//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-25.
//

module;
#include <proxy/proxy.h>

#include <string>
#include <string_view>
#include <memory>
#include <format>

module blueprint.builtin_node;
import :identity_node;
import blueprint.dyn_node;

namespace blueprint::dyn_node::builtin
{
    using namespace std::string_literals;
    std::string make_identity_id(std::string_view type_id)
    {
        return "core.identity."s + type_id.data();
    }

    std::string make_identity_name(std::string_view name)
    {
        return std::format("Identity({})", name);
    }


    identity_node_definition::identity_node_definition(type_definition_proxy ty)
        : ty_(std::move(ty)), name_(make_identity_name(ty_->name())), id_(make_identity_id(ty_->id()))
    {

    }

    id_type identity_node_definition::id() const noexcept
    {
        return id_;
    }

    text_type identity_node_definition::name() const noexcept
    {
        return name_;
    }

    text_type identity_node_definition::description() const noexcept
    {
        return "Produce the data that was inputted in, without change";
    }

    node_instance_proxy identity_node_definition::create_node() noexcept
    {
        return std::make_unique<identity_node_instance>(id_, ty_);
    }

    node_instance_proxy identity_node_definition::load(archive::input_archive_t& ar)
    {
        return create_node();
    }
    void identity_node_definition::save(archive::output_archive_t& ar, node_instance_proxy& p)
    {
        assert(id_ == p->type_id());
    }

    identity_node_instance::identity_node_instance(string_view id, const type_definition_proxy& ty, data_proxy d)
         : id_(id), sig_({ty->id()}, {ty->id()}), data_(std::move(d))
    {

    }


    id_type identity_node_instance::type_id() const noexcept
    {
        return id_;
    }

    std::span<const signature_t> identity_node_instance::signatures() const noexcept
    {
        return {&sig_, 1};
    }

    std::size_t identity_node_instance::current_variant() const noexcept
    {
        return 0;
    }

    bool identity_node_instance::set_variant(std::size_t) noexcept
    {
        return false;
    }

    bool identity_node_instance::compute(data_sequence ds) noexcept
    {
        if (ds.size() != 1)
        {
            return false;
        }
        data_ = ds[0];
        return true;
    }

    data_sequence identity_node_instance::output() const noexcept
    {
        return {data_};
    }



}