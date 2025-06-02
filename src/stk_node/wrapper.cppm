//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-29.
//

module;
#include <proxy/proxy.h>

#include <boost/callable_traits.hpp>
#include <boost/hof.hpp>
#include <boost/hana.hpp>
#include <boost/mp11.hpp>

#include <utility>
#include <concepts>
#include <memory>
#include <type_traits>

export module blueprint.stk_node:wrapper;
import blueprint.dyn_node;
import :type_mapper;

namespace blueprint::stk_node
{

    // utility


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

    template <type_mapper auto tmp, typename T>
        requires (tmp.template deducible_by_source<T>)
    constexpr dyn_node::data_proxy deduced_result_proxy(T &&t)
    {
        constexpr type_desc auto td = tmp.template deduced_by_source<T>();

        return wrap_result_data<td, T>(std::forward<T>(t));
    }

    template <type_mapper auto tmp, typename T>
        requires (tmp.template deducible_by_target<T>)
    constexpr T deduced_argument_proxy(dyn_node::data_proxy p)
    {
        type_desc auto td = tmp.template deduced_by_target<T>();

        return unwrap_parameter_data<td, T>(std::move(p));
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

    namespace details /* fun_node */
    {
        using namespace boost::mp11;
        using dyn_node::data_proxy;
        using dyn_node::data_sequence;

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
            struct valid_for_result_helper
            {
                static consteval bool valid_for_result()
                {
                    if ((hana::size(output.tuple) != 1_c))
                    {
                        return false;
                    }
                    return accept_helper<output.tuple[0_c], Rt>::acceptable;
                }
            };
            template <typename... RTS>
            struct valid_for_result_helper<std::tuple<RTS...>>
            {
                static consteval bool valid_for_result()
                {
                    if (sizeof...(RTS) != hana::size(output.tuple).value)
                    {
                        return false;
                    }
                    return hana::unpack(output.tuple, [&] (type_desc auto... x) consteval
                    {
                        return (... && accept_helper<x, RTS>::acceptable);
                    });
                }
            };

            template <typename Rt>
            static consteval bool valid_for_result()
            {
                return valid_for_result_helper<Rt>::valid_for_result();
            }

            // wrapper

            template <typename Rt>
                requires (valid_for_result<Rt>())
            static data_sequence wrap_result(Rt&& rt)
            {
                constexpr type_desc auto td = output.tuple[0_c];
                return {wrap_result_data<td, Rt>(std::forward<Rt>(rt))};
            }
            template <typename... Rts>
                requires (valid_for_result<std::tuple<Rts...>>())
            static data_sequence wrap_result(std::tuple<Rts...> rt)
            {
                auto rg = hana::make_range(hana::size_c<0>, hana::size(output.tuple));
                return hana::unpack(rg, [&](auto... i) -> data_sequence
                {
                    return {
                        wrap_result_data<output.tuple[i], Rts>(std::move(std::get<i.value>(rt)))...
                    };
                });
            }

            template <typename Rt, typename... Args>
                requires (valid_for_parameter<Args...>() && valid_for_result<Rt>())
            static std::move_only_function<std::optional<data_sequence>(data_sequence)> make_fn(std::move_only_function<Rt(Args...)> raw_fn)
            {
                return [origin = std::move(raw_fn)] (data_sequence d) -> std::optional<data_sequence>
                {
                    constexpr auto sz = hana::size(input.tuple);
                    if (d != sz.value)
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

                    return wrap_result<Rt>(hana::unpack(args, origin));
                };
            }
        };
    }

    export template <type_desc_list auto input_list, type_desc_list auto output_list>
    class /* TODO HAVE NOT TEST */ func_node
    {
        using helper = details::func_node_helper<input_list, output_list>;
    public:

        template <typename Rt, typename... Args>
            requires (helper::template valid_for_parameter<Args...>() && helper::template valid_for_result<Rt>())
        func_node(dyn_node::id_type id, dyn_node::text_type name, dyn_node::text_type description, std::move_only_function<Rt(Args...)> fn)
            : id_(id)
            , name_(name)
            , description_(description)
            , sig_(helper::signature())
            , fn_(helper::make_fn(fn))
        {}

        friend class func_node_instance;
        class func_node_instance
        {
        public:

            explicit func_node_instance(func_node &def)
                : def_(def)
            {

            }

            dyn_node::id_type type_id() const noexcept
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
            }
            [[nodiscard]] dyn_node::data_sequence output() const noexcept
            {
                return data_;
            }

        private:
            func_node &def_;
            dyn_node::data_sequence data_;
        };

        dyn_node::text_type name() const noexcept
        {
            return name_;
        }
        dyn_node::text_type description() const noexcept
        {
            return description_;
        }
        dyn_node::id_type id() const noexcept
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



    namespace details
    {

        template <type_mapper auto tmp, typename Rt>
        struct result_mapper_helper
        {
            static constexpr bool matchable = tmp.template deducible_by_source<Rt>;

            static constexpr dyn_node::output_sequence_t id_seq()
            {
                type_desc auto td = tmp.template deduced_by_source<Rt>();
                return {td.hana_id.c_str()};
            }

            static constexpr dyn_node::data_sequence result_transform(Rt r)
            {
                return {deduced_result_proxy<tmp, Rt>(std::move(r))};
            }
        };

        template <type_mapper auto tmp, typename... RTS>
        struct result_mapper_helper<tmp, std::tuple<RTS...>>
        {
            static constexpr bool matchable = (... && tmp.template deducible_by_source<RTS>);

            static constexpr dyn_node::output_sequence_t id_seq()
            {
                return {
                    (tmp.template deduced_by_source<RTS>).hana_id.c_str()...
                };
            }

            static constexpr dyn_node::data_sequence result_transform(std::tuple<RTS...> r)
            {
                using namespace boost::hana::literals;
                constexpr std::size_t sz = sizeof...(RTS);
                auto ind_range = hana::make_range(hana::size_c<0>, hana::size_c<sz>);

                return hana::unpack(ind_range, [&](auto... ind)
                {
                    return dyn_node::data_sequence{
                        result_transform<tmp, RTS>(std::move(std::get<ind.value>(r)))...
                    };
                });
            }
        };

        template <type_mapper auto tmp, typename Rt, typename... Args>
        struct deduced_func_node_helper
        {
            static constexpr bool matchable =
                result_mapper_helper<tmp, Rt>::matchable
                && (... && tmp.template deducible_by_target<Args>);

            static dyn_node::signature_t deduced_signature()
            {
                dyn_node::output_sequence_t output_seq = result_mapper_helper<tmp, Rt>::id_seq();
                dyn_node::input_sequence_t input_seq = {
                    (tmp.template deduced_by_target<Args>()).hana_id.c_str()...
                };
                return {std::move(input_seq), std::move(output_seq)};
            }
        };
    }

    export template <type_mapper auto tmp, typename Rt, typename... Args>
        requires details::deduced_func_node_helper<tmp, Rt, Args...>::matchable
    class /*TODO Reduce to func_node */ deduced_func_node
    {
        using helper = details::deduced_func_node_helper<tmp, Rt, Args...>;
        using fn_type = std::move_only_function<Rt(Args...)>;
        static constexpr std::size_t arg_sz = sizeof...(Args);
    public:
        class func_node_instance
        {
        public:
            explicit func_node_instance(deduced_func_node &d)
                : def_(d)
            {}

            [[nodiscard]] dyn_node::id_type type_id() const noexcept
            {
                return def_.id();
            }

            [[nodiscard]] std::span<const dyn_node::signature_t> signatures() const noexcept
            {
                return {&def_.sig_, 1};
            }
            [[nodiscard]] std::size_t current_variant() const noexcept
            {
                return 0;
            }
            bool set_variant(std::size_t) noexcept
            {
                return false;
            }
            bool compute(dyn_node::data_sequence ds) noexcept
            {
                if (arg_sz != ds.size())
                {
                    return false;
                }

                auto ind_pk = hana::make_range(hana::size_c<0>, hana::size_c<arg_sz>);
                auto args = hana::unpack(ind_pk, [&](auto... ind)
                {
                    return hana::make_tuple(
                        deduced_argument_proxy<tmp, Args>(std::move(ds[ind.value]))...
                    );
                });

                auto ans = hana::unpack(std::move(args), def_.fn_);

                output_ = details::result_mapper_helper<tmp, Rt>::result_transform(std::move(ans));

                return true;
            }
            [[nodiscard]] dyn_node::data_sequence output() const noexcept
            {
                return output_;
            }


        private:
            dyn_node::data_sequence output_;
            deduced_func_node &def_;
        };
        friend class func_node_instance;
        static_assert(pro::proxiable<std::unique_ptr<func_node_instance>, dyn_node::node_instance_facade>);

        deduced_func_node(fn_type fn, dyn_node::text_type name, dyn_node::text_type description, dyn_node::id_type id)
            : name_(name)
            , description_(description)
            , id_(id)
            , fn_(std::move(fn))
            , sig_(helper::deduced_signature())
        {

        }

        dyn_node::text_type name() const noexcept
        {
            return name_;
        }
        dyn_node::text_type description() const noexcept
        {
            return description_;
        }
        dyn_node::id_type id() const noexcept
        {
            return id_;
        }

        dyn_node::node_instance_proxy create_node()
        {
            return std::make_unique<func_node_instance>(*this);
        }

    private:
        dyn_node::text_type name_;
        dyn_node::text_type description_;
        dyn_node::id_type id_;
        fn_type fn_;
        dyn_node::signature_t sig_;
    };
}
