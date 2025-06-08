//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/7.
//

module;

#include <utility>
#include <string>
#include <vector>

export module blueprint.dyn_node:cpo;

namespace blueprint::dyn_node {
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

    namespace details
    {
        template <typename T>
        concept clone_member = requires(T&& t) { std::forward<T>(t).clone(); };
    }

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

    // serialize

    namespace details
    {
        template <typename T>
        concept serialize_member = requires(T&& t) { std::forward<T>(t).serialize(); };
    }

    struct serialize_t
    {
        template <details::serialize_member T>
        auto operator()(T&& t) const noexcept
        {
            return std::forward<T>(t).serialize();
        }
        template <typename T>
        auto operator()(T&& t) const noexcept
        {
            if constexpr (std::same_as<lookup_hint_t<T>, void>)
            {
                return serialize(std::forward<T>(t));
            }
            else
            {
                return serialize(lookup_hint_t<T>{}, std::forward<T>(t));
            }
        }
    };
    export constexpr serialize_t serialize;

}
