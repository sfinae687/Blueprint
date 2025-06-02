//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-29.
//

/// @file
/// The file provides the templates to wrap a callable to node.
/// TODO Clean Up Needed

module;
#include <proxy/proxy.h>

#include <boost/callable_traits.hpp>
#include <boost/hof.hpp>
#include <boost/hana.hpp>
#include <boost/mp11.hpp>

#include <boost/align/detail/element_type.hpp>
#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

export module blueprint.stk_node:wrapper;
import blueprint.dyn_node;
import :type_mapper;

namespace blueprint::stk_node
{

    // --- utility --- //

    template <type_desc auto td, typename T>
        requires details::transform_helper<td, T>::transformable
    T unwrap_parameter_data(dyn_node::data_proxy d)
    {
        dyn_node::id_type td_id = td.hana_id.c_str();
        assert(td_id == d->type_id());
        using data_type = decltype(+td.data_type)::type;

        auto &&data = proxy_cast<data_type&>(*d);
        return transform<td, T>(data);
    }

    template <type_desc auto td, typename T>
        requires details::accept_helper<td, T>::acceptable
    dyn_node::data_proxy wrap_result_data(T&& t)
    {
        using data_type = decltype(+td.data_type)::type;
        return std::make_shared<data_type>(accept<td>(std::forward<T>(t)));
    }

    // type-desc list

    namespace details /* type-desc list */
    {
        template <type_desc auto... td>
        struct basic_type_desc_list
        {
            static constexpr auto tuple = hana::make_tuple(td...);
        };

    }

    export template <type_desc auto... td>
    struct input_type_list_t : details::basic_type_desc_list<td...>
    {};
    export template <type_desc auto... td>
    constexpr input_type_list_t<td...> input_type;

    export template <type_desc auto... td>
    struct output_type_list_t : details::basic_type_desc_list<td...>
    {};
    export template <type_desc auto... td>
    constexpr output_type_list_t<td...> output_type;

    template <typename T>
    concept type_desc_list = requires (T t) {
        t.tuple;
        // TODO more strict constraint
    };

    // --- func_node --- //

    namespace details /* func_node */
    {
        using namespace boost::mp11;
        using dyn_node::data_proxy;
        using dyn_node::data_sequence;

        // return value process

        template <typename Rt>
        consteval auto result_list(hana::basic_type<Rt>)
        {
            return hana::make_tuple(hana::type_c<Rt>);
        }

        template <typename... Rts>
        consteval auto result_list(hana::basic_tuple<std::tuple<Rts...>>)
        {
            return hana::make_tuple(hana::type_c<Rts>...);
        }

        template <typename Rt>
        consteval auto result_list(hana::basic_tuple<std::optional<Rt>>)
        {
            return result_list(hana::type_c<Rt>);
        }

        template <typename Rt>
        constexpr bool result_success(Rt &&rt)
        {
            return true;
        }
        template <typename Rt>
        bool result_success(std::optional<Rt> &&rt)
        {
            return rt.has_value();
        }

        template <std::size_t i, typename Rt>
            requires (i == 0)
        constexpr decltype(auto) result_decompose(Rt& rt)
        {
            return std::move(rt);
        }

        template <std::size_t i, typename... Rts>
            requires (i < sizeof...(Rts))
        constexpr decltype(auto) result_decompose(std::tuple<Rts...> &rt)
        {
            return std::move(std::get<i>(rt));
        }

        template <std::size_t i, typename Rt>
        constexpr decltype(auto) result_decompose(std::optional<Rt>& rt)
        {
            return result_decompose<i>(rt.value());
        }


        // func node helper

        template <type_desc_list auto input, type_desc_list auto output>
        struct func_node_helper
        {

            // signature

            static dyn_node::signature_t signature()
            {
                auto input_id = hana::unpack(input.tuple, [&](type_desc auto... x)
                {
                    return dyn_node::input_sequence_t{x.hana_id.c_str()...};
                });
                auto output_id = hana::unpack(output.tuple, [&](type_desc auto... x)
                {
                    return dyn_node::output_sequence_t{x.hana_id.c_str()...};
                });
                return {std::move(input_id), std::move(output_id)};
            }

            // compile check

            template <typename... T>
            static consteval bool valid_for_parameter()
            {
                if (sizeof...(T) != hana::size(input.tuple).value)
                {
                    return false;
                }
                return hana::unpack(input.tuple, [&] (type_desc auto... x) consteval
                {
                    return (... && transform_helper<x, T>::transformable);
                });
            }

            template <typename Rt>
            static consteval bool valid_for_result()
            {
                constexpr auto element_list = result_list(hana::type_c<Rt>);
                if constexpr (hana::size(element_list) != hana::size(output.tuple))
                {
                    return false;
                }

                return hana::fold_left(hana::zip(output.tuple, element_list), true, [&](auto s, auto x) consteval
                {
                    constexpr auto td = x[0_c];
                    constexpr auto e = x[1_c];
                    using e_type = decltype(+e)::type;
                    return s && accept_helper<td, e_type>::acceptable;
                });
            }

            // wrapper

            template <typename Rt>
                requires (valid_for_result<Rt>())
            static data_sequence wrap_result(Rt&& rt)
            {
                assert(result_success(rt));
                auto rg = hana::make_range(hana::size_c<0>, hana::size(output.tuple));
                auto ele = result_list(hana::type_c<Rt>);
                return hana::unpack(rg, [&](auto... i) -> data_sequence
                {
                    return {
                        wrap_result_data<output.tuple[i], typename decltype(+ele[i])::type>(result_decompose<i.value>(rt))...
                    };
                });
            }

            template <typename Rt, typename... Args>
                requires (valid_for_parameter<Args...>() && valid_for_result<Rt>())
            static std::move_only_function<std::optional<data_sequence>(data_sequence)>
            make_fn(std::move_only_function<Rt(Args&&...)> raw_fn)
            {
                return [origin = std::move(raw_fn)] (data_sequence d) mutable -> std::optional<data_sequence>
                {
                    constexpr auto sz = hana::size(input.tuple);
                    if (d.size() != sz.value)
                    {
                        return std::nullopt;
                    }

                    constexpr auto rg = hana::make_range(hana::size_c<0>, sz);
                    auto args = hana::unpack(rg, [&](auto... i)
                    {
                        return hana::make_tuple(
                            unwrap_parameter_data<input.tuple[i], Args>(std::move(d[i.value]))...
                        );
                    });

                    auto result = hana::unpack(std::move(args), [&]<typename... TS> (TS&&... x)
                    {
                        return origin(std::forward<TS>(x)...);
                    });

                    if (! result_success(result))
                    {
                        return std::nullopt;
                    }

                    return wrap_result<Rt>(std::move(result));
                };
            }
        };
    }

    export template <type_desc_list auto input_list, type_desc_list auto output_list>
    class func_node
    {
        using helper = details::func_node_helper<input_list, output_list>;
    public:

        template <typename Rt, typename... Args>
            requires (helper::template valid_for_parameter<Args...>() && helper::template valid_for_result<Rt>())
        func_node(dyn_node::id_type id, dyn_node::text_type name, dyn_node::text_type description, std::move_only_function<Rt(Args&&...)> fn)
            : id_(id)
            , name_(name)
            , description_(description)
            , sig_(helper::signature())
            , fn_(helper::make_fn(std::move(fn)))
        {}

        friend class func_node_instance;
        class func_node_instance
        {
        public:

            explicit func_node_instance(func_node &def)
                : def_(def)
            {

            }

            [[nodiscard]] dyn_node::id_type type_id() const noexcept
            {
                return def_.id_;
            }

            [[nodiscard]] std::span<const dyn_node::signature_t> signatures() const noexcept
            {
                return {&def_.sig_, 1};
            }
            [[nodiscard]] std::size_t current_variant() const noexcept
            {
                return 0;
            }
            [[nodiscard]] bool set_variant(std::size_t) const noexcept
            {
                return false;
            }
            [[nodiscard("Check the state of compute is always neccessary")]]
            bool compute(dyn_node::data_sequence in_data) noexcept
            {
                auto rt = def_.fn_(in_data);
                if (!rt)
                {
                    return false;
                }
                data_ = std::move(*rt);
                return true;
            }
            [[nodiscard]] dyn_node::data_sequence output() const noexcept
            {
                return data_;
            }

        private:
            func_node &def_;
            dyn_node::data_sequence data_{};
        };

        [[nodiscard]] dyn_node::text_type name() const noexcept
        {
            return name_;
        }
        [[nodiscard]] dyn_node::text_type description() const noexcept
        {
            return description_;
        }
        [[nodiscard]] dyn_node::id_type id() const noexcept
        {
            return id_;
        }

        dyn_node::node_instance_proxy create_node()
        {
            return std::make_shared<func_node_instance>(*this);
        }

    private:
        dyn_node::id_type id_;
        dyn_node::text_type name_, description_;
        dyn_node::signature_t sig_;
        std::move_only_function<std::optional<dyn_node::data_sequence>(dyn_node::data_sequence)> fn_;
    };


    // deduced func node

    namespace details /* deduced func node helper */
    {

        template <type_mapper auto tmp, typename... Args>
            requires (... && tmp.template deducible_by_target<Args>)
        consteval type_desc_list auto deduced_type_list()
        {
            return input_type<
                tmp.template deduced_by_target<Args>()...
            >;
        }

        template <type_mapper auto tmp, typename Rt>
        consteval bool result_deducible()
        {
            auto rt_list = result_list(hana::type_c<Rt>);
            return hana::unpack(rt_list, [&] (auto... x)
            {
                return (... && tmp.template deducible_by_source<typename decltype(+x)::type>);
            });
        }

        template <type_mapper auto tmp, typename Rt>
            requires (result_deducible<tmp, Rt>())
        consteval type_desc_list auto deduced_result_list()
        {
            auto rt_list = result_list(hana::type_c<Rt>);
            return hana::unpack(rt_list, [&](auto... x)
            {
                return deduced_type_list<tmp, typename decltype(+x)::type...>();
            });
        }


        template <type_mapper auto tmp, typename Rt, typename... Args>
        struct deduced_func_node_helper
        {
            static constexpr bool matchable =
                result_deducible<tmp, Rt>()
                && (... && tmp.template deducible_by_target<Args>);
        };
    }

    export template <type_mapper auto tmp, typename Rt, typename... Args>
        requires details::deduced_func_node_helper<tmp, Rt, Args...>::matchable
    class deduced_func_node
        : public func_node<
            details::deduced_type_list<tmp, Args...>(),
            details::deduced_result_list<tmp, Rt>()
        >
    {
        using fn_type = std::move_only_function<Rt(Args&&...)>;
        using base  = func_node<
            details::deduced_type_list<tmp, Args...>(),
            details::deduced_result_list<tmp, Rt>()
        >;
    public:
        deduced_func_node(fn_type fn, dyn_node::text_type name, dyn_node::text_type description, dyn_node::id_type id)
            : base(id, name, description, std::move(fn))
        {

        }

    };
}
