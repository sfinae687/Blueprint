//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-29.
//

module;
#include <proxy.h>

#include <boost/callable_traits.hpp>
#include <boost/hof.hpp>
#include <boost/hana.hpp>
#include <boost/mp11.hpp>

#include <utility>
#include <concepts>
#include <memory>

export module blueprint.stk_node:wrapper;
import blueprint.dyn_node;
import :type_mapper;

namespace blueprint::stk_node
{

    template <type_mapper auto tmp, typename T>
        requires (tmp.template deducible_by_source<T>)
    constexpr dyn_node::data_proxy result_proxy(T &&t)
    {
        type_desc auto td = tmp.template deduced_by_source<T>();

        using data_type = typename decltype(+td.data_type)::type;
        return std::make_shared<data_type>(accept.operator()<td>(std::forward<T>(t)));
    }

    template <type_mapper auto tmp, typename T>
        requires (tmp.template deducible_by_target<T>)
    constexpr T argument_proxy(dyn_node::data_proxy p)
    {
        type_desc auto td = tmp.template deduced_by_target<T>();
        constexpr dyn_node::id_type id = td.hana_id.c_str();
        assert(p->type_id() == id);

        using data_type = typename decltype(+td.data_type)::type;
        data_type &dd = proxy_cast<data_type&>(*p);
        return transform.operator()<td, T>(dd);
    }

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
                return {result_proxy<tmp, Rt>(std::move(r))};
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
                        result_proxy<tmp, RTS>(std::move(std::get<ind.value>(r)))...
                    };
                });
            }
        };

        template <type_mapper auto tmp, typename Rt, typename... Args>
        struct func_node_helper
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
        requires details::func_node_helper<tmp, Rt, Args...>::matchable
    class deduced_func_node
    {
        using helper = details::func_node_helper<tmp, Rt, Args...>;
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
                        argument_proxy<tmp, Args>(std::move(ds[ind.value]))...
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
