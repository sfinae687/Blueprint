//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-29.
//

module;
#include <proxy/proxy.h>

#include <boost/hana.hpp>
#include <boost/hana/string.hpp>

#include <string_view>

export module blueprint.stk_node:type_mapper;
import blueprint.dyn_node;

namespace blueprint::stk_node
{
    namespace hana = boost::hana;
    using namespace boost::hana::literals;

    export template<char const * ID, typename DATA, typename DEF>
    struct basic_type_desc
    {
        static constexpr auto hana_id = hana::to_string(hana::integral_constant<char const *, ID>{});
        using hana_id_type = decltype(hana_id);

        static constexpr auto hana_tuple = hana::make_tuple(
            hana_id,
            hana::type_c<DATA>,
            hana::type_c<DEF>
        );

        static constexpr auto data_type = hana::type_c<DATA>;
        static constexpr auto def_type = hana::type_c<DEF>;

        static constexpr std::string_view id = ID;
        using std_tuple = std::tuple<hana_id_type, DATA, DEF>;
        using data_t = DATA;
        using def_t = DEF;

    };


    export template <typename T>
    concept type_desc = requires (T t)
    {
        t.hana_id;
        t.data_type;
        t.def_type;
    };

    namespace details
    {
        template <type_desc auto td, typename T>
        struct transform_helper
        {
            using data_type = typename decltype(+td.data_type)::type;
            using basic_T = std::remove_cvref_t<T>;

            static constexpr bool exactly_match = std::same_as<data_type, basic_T>;

            static constexpr bool mem_transform = requires (data_type &d)
            {
                { td.template transform<T>(d) } -> std::same_as<T>;
            };

            static constexpr bool free_transform = requires (data_type &d)
            {
                { transform<T>(td, d) } -> std::same_as<T>;
            };

            static constexpr bool transformable = exactly_match || mem_transform || free_transform;
        };
    }

    export struct transform_t
    {
        template <type_desc auto td, typename T>
            requires details::transform_helper<td, T>::transformable
        T operator() (typename decltype(+td.data_type)::type &d) const
        {
            using helper = details::transform_helper<td, T>;
            if constexpr (helper::exactly_match)
            {
                return d;
            }
            else if constexpr (helper::mem_transform)
            {
                return td.template transform<T>(d);
            }
            else if constexpr (helper::free_transform)
            {
                return transform<T>(td, d);
            }
            __builtin_unreachable();
        }
    };
    export constexpr transform_t transform{};

    namespace details
    {
        template <type_desc auto td, typename S>
        struct accept_helper
        {
            using data_type = typename decltype(+td.data_type)::type;
            using basic_source_type = std::remove_cvref_t<S>;

            static constexpr bool exactly_match = std::same_as<data_type, basic_source_type>;

            static constexpr bool free_accept = requires (S s)
            {
                { accept(td, s) } -> std::same_as<data_type>;
            };

            static constexpr bool mem_accept = requires (S s)
            {
                { td.accept(s) } -> std::same_as<data_type>;
            };

            static constexpr bool acceptable = exactly_match || free_accept || mem_accept;
        };
    }

    export struct accept_t
    {
        template <type_desc auto td, typename S>
            requires details::accept_helper<td, S>::acceptable
        typename details::accept_helper<td, S>::data_type operator() (S &&s) const
        {
            using helper = details::accept_helper<td, S>;
            if constexpr (helper::exactly_match)
            {
                return {std::forward<S>(s)};
            }
            else if constexpr (helper::mem_accept)
            {
                return td.accept(std::forward<S>(s));
            }
            else if constexpr (helper::free_accept)
            {
                return accept(td, std::forward<S>(s));
            }
            __builtin_unreachable();
        }
    };
    export constexpr accept_t accept{};



    namespace details
    {
        template <type_desc auto... DS>
        struct mapper_helper
        {
            static constexpr auto ds_tuple = hana::make_tuple(DS...);

            template <typename T>
            static constexpr auto transformable_ds = hana::filter(ds_tuple, [] (auto x) constexpr
            {
                using helper = details::transform_helper<x, T>;
                return hana::bool_c<helper::transformable>;
            });

            template <typename T>
            static constexpr auto acceptable_ds = hana::filter(ds_tuple, [] (auto x) constexpr {
                using helper = details::accept_helper<x, T>;
                return hana::bool_c<helper::acceptable>;
            });

        };
    }

    export template <type_desc auto... DS>
    class mapper
    {
        using helper = details::mapper_helper<DS...>;
    public:

        template <typename T>
        static constexpr bool deducible_by_target = (! hana::is_empty(helper::template transformable_ds<T>)).value;
        template <typename T>
        static constexpr bool deducible_by_source = (! hana::is_empty(helper::template acceptable_ds<T>)).value;

        template<typename T>
            requires deducible_by_target<T>
        constexpr auto deduced_by_target() const
        {
            using namespace boost::hana::literals;
            constexpr auto sel_seq = helper::template transformable_ds<T>;
            return sel_seq[0_c];
        }

        template <typename T>
            requires deducible_by_source<T>
        constexpr auto deduced_by_source() const
        {
            using namespace boost::hana::literals;
            constexpr auto sel_seq = helper::template acceptable_ds<T>;
            return sel_seq[0_c];
        }
    };

    namespace details
    {
        template <typename T>
        struct is_mapper : std::false_type {};

        template <type_desc auto... DS>
        struct is_mapper<mapper<DS...>> : std::true_type {};
    }

    template <typename T>
    concept type_mapper = details::is_mapper<T>::value;

}