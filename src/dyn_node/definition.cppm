//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//

module;
#include <boost/hana/detail/create.hpp>
#include <proxy.h>

#include <unordered_map>
#include <vector>
#include <type_traits>

export module blueprint.dyn_node:definition;

namespace blueprint::dyn_node
{
    export using text_type = std::string_view;
    export using id_type = std::string_view;

    // Data operation

    export template <typename T>
    struct lookup_hint
    {
        using type = void;
    };
    template <typename T>
    using lookup_hint_t = typename lookup_hint<T>::type;

    namespace details
    {
        template <typename T>
        concept type_id_member = requires (T t)
        {
            t.type_id();
        };
        template <typename T>
        concept clone_member = requires (T t)
        {
            t.clone();
        };
    }

    struct type_id_t
    {
        template <details::type_id_member T>
        id_type operator() (T t) const
        {
            return t.type_id();
        }

        template <typename T>
        id_type operator() (T t) const
        {
            if constexpr (std::same_as<lookup_hint_t<T>, void>)
            {
                return type_id(std::move(t));
            }
            else
            {
                return type_id(lookup_hint_t<T>{}, std::move(t));
            }
        }
    };
    export constexpr type_id_t type_id;

    struct clone_t
    {
        template <details::clone_member T>
        auto operator() (T t) const
        {
            return t.clone();
        }
        template <typename T>
        auto operator() (T t) const
        {
            if constexpr (std::same_as<lookup_hint_t<T>, void>)
            {
                return clone(std::move(t));
            }
            else
            {
                return clone(lookup_hint_t<T>{}, std::move(t));
            }
        }
    };
    export constexpr clone_t clone;

    // Type

    PRO_DEF_MEM_DISPATCH(name_dispatch, name);
    PRO_DEF_MEM_DISPATCH(description_dispatch, description);
    PRO_DEF_MEM_DISPATCH(id_dispatch, id, id);

    export struct type_definition_facade : pro::facade_builder
        ::add_convention<name_dispatch, text_type() const>
        ::add_convention<description_dispatch, text_type() const>
        ::add_convention<id_dispatch, id_type() const>
        ::support_destruction<pro::constraint_level::nontrivial>
        ::support_rtti
        ::build
    {};

    PRO_DEF_FREE_AS_MEM_DISPATCH(type_id_dispatch, dyn_node::type_id, type_id);
    PRO_DEF_FREE_AS_MEM_DISPATCH(clone_dispatch, dyn_node::clone, clone);

    template <typename F>
    using copy_overload = pro::proxy<F>() const;

    export struct data_interface_facade : pro::facade_builder
        ::add_convention<type_id_dispatch, id_type() const>
        ::add_convention<clone_dispatch, pro::facade_aware_overload_t<copy_overload>>
        ::support_copy<pro::constraint_level::nontrivial>
        ::support_destruction<pro::constraint_level::nontrivial>
        ::support_relocation<pro::constraint_level::nontrivial>
        ::support_rtti
        ::support_direct_rtti
        ::build
    {};

    export using type_definition_proxy = pro::proxy<type_definition_facade>;
    export using data_proxy = pro::proxy<data_interface_facade>;


    // Node

    PRO_DEF_MEM_DISPATCH(channels_dispatch, channels);
    PRO_DEF_MEM_DISPATCH(set_channel_dispatch, set_channel);
    PRO_DEF_MEM_DISPATCH(get_channel_dispatch, get_channel);
    PRO_DEF_MEM_DISPATCH(compute_dispatch, compute);
    PRO_DEF_MEM_DISPATCH(outputs_dispatch, outputs);
    PRO_DEF_MEM_DISPATCH(get_output_dispatch, get_output);


    // Node Instance
    //
    // - type_id(): Acquire the type id of the instance
    // - channels(): Get the type id vector of channels
    // - set_channel(data_proxy, std::size_t): Set the data of the specified input channel
    // - get_channel() -> data_proxy : Get the data of specified input channel.
    //  The node instance should accept the modification of data returned by get_channel
    // - compute(): Update output with input.
    //  If output is not available, nullptr returned.
    // - outputs(): The type of output channels.
    // - get_output(std::size_t): The specified output data.
    //  It could not be mutable.

    export struct node_instance_facade : pro::facade_builder
            ::add_convention<type_id_dispatch, id_type() const>
            ::add_convention<channels_dispatch, std::vector<id_type>() const>
            ::add_convention<set_channel_dispatch, bool(std::size_t, data_proxy)>
            ::add_convention<get_channel_dispatch, data_proxy(std::size_t) const>
            ::add_convention<compute_dispatch, void()>
            ::add_convention<outputs_dispatch, std::vector<id_type>() const>
            ::add_convention<get_output_dispatch, data_proxy(std::size_t) const>
            ::support_destruction<pro::constraint_level::nontrivial>
            ::support_rtti
            ::build
        {};

    PRO_DEF_MEM_DISPATCH(create_node_dispatch, create_node);

    export struct node_definition_facade : pro::facade_builder
        ::add_convention<name_dispatch, text_type() const>
        ::add_convention<description_dispatch, text_type() const>
        ::add_convention<id_dispatch, id_type() const>
        ::add_convention<create_node_dispatch, pro::proxy<node_instance_facade>()>
        ::support_destruction<pro::constraint_level::nontrivial>
        ::build
    {};

    export using node_definition_proxy = pro::proxy<node_definition_facade>;
    export using node_instance_proxy = pro::proxy<node_instance_facade>;

    export using type_definitions_t = std::unordered_map<id_type, type_definition_proxy>;
    export using node_definitions_t = std::unordered_map<id_type, node_definition_proxy>;
}

