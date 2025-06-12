//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//

module;
#include <boost/hana/detail/create.hpp>

#include <proxy/proxy.h>

#include <any>
#include <memory>
#include <span>
#include <type_traits>
#include <unordered_map>
#include <vector>

export module blueprint.dyn_node:definition;
export import blueprint.archive;

namespace blueprint::dyn_node
{
    export using text_type = std::string_view;
    export using id_type = std::string_view;

    // predefined type
    export constexpr id_type undefined_type = "core.undefined";

    // type alias

    export {
        using input_sequence_t = std::pmr::vector<id_type>;
        using output_sequence_t = std::pmr::vector<id_type>;
        struct signature_t
        {
            input_sequence_t input;
            output_sequence_t output;
        };
    }

    /// @defgroup Nodes And Types
    /// @{
    ///
    /// A Node is a object that could receive multi input date, and then produces multi output, behave like a function.
    /// The input and output of a node is called as `channel`. The node owns thee date of its output channel. And the
    /// node will reserve its output date until it is commanded to recompute output from the given inputs.
    ///
    /// A node may have multi variants that represents the multi mapping-ship. The current variants is a inner state
    /// maintained by node instance. So for a node with fixed variant, it input and output type is immediately known
    /// regardless of the nodes its connected to. In fact, node instance does not maintain the link relationship.
    /// The feature is aiming to provide the multi interface for a **same** operation.
    ///
    /// @}

    // CPO

    // TODO `noexcept` process.

    export template <typename T>
    struct lookup_hint
    {
        using type = void;
    };
    template <typename T>
    using lookup_hint_t = lookup_hint<std::remove_cvref_t<T>>::type;

    // Concept For CPO

    namespace details
    {
        template <typename T>
        concept type_id_member = requires(T&& t) { std::forward<T>(t).type_id(); };
        template <typename T>
        concept clone_member = requires(T&& t) { std::forward<T>(t).clone(); };
    } // namespace details

    // type_id

    struct type_id_t
    {
        template <typename T>
        id_type operator()(T&& t) const noexcept
        {
            if constexpr (details::type_id_member<T>)
            {
                return std::forward<T>(t).type_id();
            }
            else if constexpr (std::same_as<lookup_hint_t<T>, void>)
            {
                return type_id(std::forward<T>(t));
            }
            else
            {
                return type_id(lookup_hint_t<T>{}, std::forward<T>(t));
            }
        }
    };
    export constexpr type_id_t type_id;

    // clone

    struct clone_t
    {
        template <details::clone_member T>
        auto operator()(T&& t) const noexcept
        {
            return std::forward<T>(t).clone();
        }
        template <typename T>
        auto operator()(T&& t) const noexcept
        {
            if constexpr (std::same_as<lookup_hint_t<T>, void>)
            {
                return clone(std::forward<T>(t));
            }
            else
            {
                return clone(lookup_hint_t<T>{}, std::forward<T>(t));
            }
        }
    };
    export constexpr clone_t clone;

    /// @todo Generic Type Support.
    // is_generic

    namespace details
    {
        template <typename T>
        concept is_generic_member = requires(const T t) { t.is_generic(); };
        template <typename T>
        concept is_generic_free = requires(const T t) { is_generic(t); };
        template <typename T>
        concept is_generic_free_with_hint = requires(const T t) { is_generic(t, lookup_hint_t<T>{}); };
        template <typename T>
        concept is_generic_defined = is_generic_member<T> || is_generic_free<T> || is_generic_free_with_hint<T>;
    } // namespace details


    PRO_DEF_MEM_DISPATCH(name_dispatch, name);
    PRO_DEF_MEM_DISPATCH(description_dispatch, description);
    PRO_DEF_MEM_DISPATCH(id_dispatch, id, id);

    PRO_DEF_FREE_AS_MEM_DISPATCH(type_id_dispatch, dyn_node::type_id, type_id);
    PRO_DEF_FREE_AS_MEM_DISPATCH(clone_dispatch, dyn_node::clone, clone);

    // Data proxy has reference meaning and reference counter (to redeclare memory) in it.
    // Other proxies have the ownship of its object.

    template <typename F>
    using copy_overload = pro::proxy<F>() const;
    template <typename F>
    using equal_to_overload = bool(const pro::proxy_indirect_accessor<F> &) const;

    export struct data_interface_facade : pro::facade_builder
        ::add_convention<type_id_dispatch, id_type() const>
        ::add_convention<clone_dispatch, pro::facade_aware_overload_t<copy_overload>>
        ::support_copy<pro::constraint_level::nontrivial>
        ::support_destruction<pro::constraint_level::nontrivial>
        ::support_relocation<pro::constraint_level::nontrivial>
        ::support<pro::skills::rtti>
        ::build
    { };

    // Type

    using generic_parameter = std::pmr::unordered_map<id_type, std::any>;

    // PRO_DEF_FREE_AS_MEM_DISPATCH(save_dispatch, archive::save, save);
    // PRO_DEF_FREE_AS_MEM_DISPATCH(load_dispatch, archive::load, load);

    PRO_DEF_MEM_DISPATCH(save_dispatch, save);
    PRO_DEF_MEM_DISPATCH(load_dispatch, load);

    export struct type_definition_facade : pro::facade_builder
        ::add_convention<name_dispatch, text_type() const>
        ::add_convention<description_dispatch, text_type() const>
        ::add_convention<id_dispatch, id_type() const>
        ::add_convention<load_dispatch, pro::proxy<data_interface_facade>(archive::input_archive_t &)>
        ::add_convention<save_dispatch, void(archive::output_archive_t &, pro::proxy<data_interface_facade> &)>
        ::support_copy<pro::constraint_level::nontrivial>
        ::support_relocation<pro::constraint_level::nontrivial>
        ::support_destruction<pro::constraint_level::nontrivial>
        ::support<pro::skills::rtti>
        ::build
    { };

    export using type_definition_proxy = pro::proxy<type_definition_facade>;
    export using data_proxy = pro::proxy<data_interface_facade>;
    export using data_sequence = std::pmr::vector<data_proxy>;


    // Node

    /// @defgroup Node in Dynamic layer
    /// @{
    /// - signatures() -> std::span<signature_t> const noexcept : express all variant of the node.
    /// - current_variant() -> std::size_t const noexcept
    /// - set_variant(std::size_t) -> void noexcept
    /// - type_id() -> id_type const noexcept : get the type id of node
    /// - compute(data_sequence) -> void : process given data
    /// - output() -> data_seqence : get the last process result.
    /// @}


    PRO_DEF_MEM_DISPATCH(compute_dispatch, compute);
    PRO_DEF_MEM_DISPATCH(signatures_dispatch, signatures);
    PRO_DEF_MEM_DISPATCH(current_variant_dispatch, current_variant);
    PRO_DEF_MEM_DISPATCH(set_variant_dispatch, set_variant);
    PRO_DEF_MEM_DISPATCH(output_dispatch, output);


    export struct node_instance_facade : pro::facade_builder
        ::add_convention<type_id_dispatch, id_type() const noexcept>
        ::add_convention<signatures_dispatch, std::span<const signature_t>() const noexcept>
        ::add_convention<current_variant_dispatch, std::size_t() const noexcept>
        ::add_convention<set_variant_dispatch, bool(std::size_t) noexcept>
        ::add_convention<compute_dispatch, bool(data_sequence) noexcept>
        ::add_convention<output_dispatch, data_sequence() const noexcept>
        ::support_destruction<pro::constraint_level::nontrivial>
        ::support<pro::skills::rtti>
        ::build
    {};

    PRO_DEF_MEM_DISPATCH(create_node_dispatch, create_node);

    export struct node_definition_facade : pro::facade_builder
        ::add_convention<name_dispatch, text_type() const noexcept>
        ::add_convention<description_dispatch, text_type() const noexcept>
        ::add_convention<id_dispatch, id_type() const noexcept>
        ::add_convention<create_node_dispatch, pro::proxy<node_instance_facade>()>
        ::add_convention<load_dispatch, pro::proxy<node_instance_facade>(archive::input_archive_t &)>
        ::add_convention<save_dispatch, void(archive::output_archive_t &, pro::proxy<node_instance_facade> &)>
        ::support_copy<pro::constraint_level::nontrivial>
        ::support_relocation<pro::constraint_level::nontrivial>
        ::support_destruction<pro::constraint_level::nontrivial>
        ::build
    {};

    export using node_definition_proxy = pro::proxy<node_definition_facade>;
    export using node_instance_proxy = pro::proxy<node_instance_facade>;

    export using type_definitions_t = std::unordered_map<id_type, type_definition_proxy>;
    export using node_definitions_t = std::unordered_map<id_type, node_definition_proxy>;

    export
    {
        template <typename T>
        concept node_definition = pro::proxiable<std::shared_ptr<T>, node_definition_facade>;
        template <typename T>
        concept node_instance = pro::proxiable<std::unique_ptr<T>, node_instance_facade>;
        template <typename T>
        concept type_definition = pro::proxiable<std::shared_ptr<T>, type_definition_facade>;
        template <typename T>
        concept data = pro::proxiable<std::shared_ptr<T>, data_interface_facade>;
    }
} // namespace blueprint::dyn_node
