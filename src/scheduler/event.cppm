//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-29.
//

module;
#include <boost/lockfree/queue.hpp>
#include <boost/mp11/mpl.hpp>
#include <boost/mp11/map.hpp>
#include <boost/hana.hpp>
#include <boost/callable_traits/args.hpp>

#include <concepts>
#include <utility>

export module blueprint.scheduler:event;

namespace blueprint::scheduler
{
    // Event

    namespace details
    {
        template <typename T>
        concept event_data = std::movable<T>;
    }

    export template <typename T>
    concept event = requires(T t)
        {
            requires details::event_data<typename T::data_type>;
        } && requires(typename T::data_type d)
        {
            { d = std::move(d) } noexcept;
        };

    export template <event E>
    struct event_data
    {
        using type = typename E::data_type;
    };

    export template <event E>
    using event_data_t = typename event_data<E>::type;

    // Event Queue

    // TODO configable queue size

    export constexpr std::size_t default_queue_size = 64;

    namespace details
    {
        using namespace boost::mp11;
        namespace hana = boost::hana;
        using std::remove_cvref_t;


        template <event... ES>
        struct event_queue_helper
        {
            // List

            using event_type_list = mp_unique<
                mp_transform<std::remove_cvref_t, mp_list<ES...>>
            >;

            using data_type_list = mp_transform<event_data_t, event_type_list>;

            // Map between event and event data

            template<typename MP, event E>
            using find_and_push_back = mp_map_replace<
                MP,
                mp_push_back<
                    mp_map_find<MP, event_data_t<E>>,
                    E
                >
            >;

            template <typename MP, event E>
            using insert_event_type = mp_eval_if_not<
                mp_map_contains<MP, event_data_t<E>>,
                mp_push_back<MP, mp_list<event_data_t<E>, E>>,
                find_and_push_back,
                MP,
                E
            >;

            struct event1
            {
                using data_type = int;
            };

            template <typename L>
            using is_single_event_data = mp_bool<(mp_size<L>::value == 2)>;

            using data_type_to_event = mp_fold<event_type_list, mp_list<>, insert_event_type>;
            using direct_seekable_data_type = mp_copy_if<data_type_to_event, is_single_event_data>;

            template <typename D>
                requires mp_map_contains<direct_seekable_data_type, D>::value
            using data_to_event = mp_second<mp_map_find<direct_seekable_data_type, D>>;

            // Type Alias

            using queue_map = mp_rename<
                mp_transform<
                    hana::pair,
                    mp_transform<
                        hana::type,
                        event_type_list
                    >,
                    mp_transform_q<
                        mp_compose_q<
                            mp_quote<event_data_t>,
                            mp_bind<
                                boost::lockfree::queue,
                                _1,
                                boost::lockfree::capacity<default_queue_size>
                            >
                        >,
                        event_type_list
                    >
                >,
                hana::map
            >;

            // Check

            template <typename T>
            static constexpr bool among_us = mp_contains<event_type_list, T>::value;

            template <typename T>
            static constexpr bool among_data = mp_contains<data_type_list, T>::value;

            template <typename T>
            static constexpr bool among_deduced_data = mp_map_contains<direct_seekable_data_type, T>::value;


            template <typename Fn, event E>
                requires among_us<E>
            static constexpr bool invocable_with_event_L1 = std::same_as<
                std::tuple_element_t<0, boost::callable_traits::args_t<Fn>>,
                event_data_t<E>
            >;

            template <typename Fn, event E>
                requires among_us<E>
            static constexpr bool invocable_with_event = std::invocable<Fn, event_data_t<E>>;

            template <typename Fn, event E>
                requires among_us<E>
            using mp_invocable_with_event_L1 = mp_bool<invocable_with_event_L1<Fn, E>>;

            template <typename Fn, event E>
                requires among_us<E>
            using mp_invocable_with_event = mp_bool<invocable_with_event<Fn, E>>;

            template <typename Fn>
            static constexpr bool deduced_invocable_L1 = mp_count_if_q<
                event_type_list,
                mp_bind<mp_invocable_with_event_L1, Fn, _1>
            >::value == 1;

            template <typename Fn>
            static constexpr bool deduced_invocable = deduced_invocable_L1<Fn> || mp_count_if_q<
                event_type_list,
                mp_bind<mp_invocable_with_event, Fn, _1>
            >::value == 1;

            template <typename Fn>
            static constexpr bool invocable_for_all = mp_all_of_q<
                event_type_list,
                mp_bind<mp_invocable_with_event, Fn, _1>
            >::value;

            template <typename Fn, event E>
                requires among_us<E>
            static constexpr bool tagged_invocable_with_event =
                std::invocable<Fn, hana::type<E>, event_data_t<E>> || std::invocable<Fn, event_data_t<E>, hana::type<E>>;

            // Deduced invocable

            template <typename Fn>
                requires deduced_invocable<Fn>
            using deduced_event_to_invoke = mp_if_c<
                deduced_invocable_L1<Fn>,
                mp_at<
                    event_type_list,
                    mp_find_if_q<
                        event_type_list,
                        mp_bind<mp_invocable_with_event_L1, Fn, _1>
                    >
                >,
                mp_at<
                    event_type_list,
                    mp_find_if_q<
                        event_type_list,
                        mp_bind<mp_invocable_with_event, Fn, _1>
                    >
                >
            >;

            // fn

            template <event E, typename Fn>
                requires tagged_invocable_with_event<Fn, E>
            void tagged_invoke(Fn &&f, event_data_t<E> d)
            {
                using call_arg = boost::callable_traits::args<Fn>;
                if constexpr (std::same_as<std::tuple_element_t<0, call_arg>, hana::type<E>>)
                {
                    std::invoke(std::forward<Fn>(f), hana::type_c<E>, std::move(d));
                } else
                {
                    std::invoke(std::forward<Fn>(f), std::move(d), hana::type_c<E>);
                }
            }

        };
    }

    export template <event... ES>
    class event_queue
    {
        using helper_ = details::event_queue_helper<ES...>;

        // helper function

        template <event E>
            requires helper_::template among_us<E>
        auto&& get_queue()
        {
            using boost::hana::type_c;
            return queue_map_[type_c<E>];
        }

    public:
        event_queue() = default;
        event_queue(const event_queue&) = delete;
        event_queue& operator=(const event_queue&) = delete;
        event_queue(event_queue&&) noexcept = default;
        event_queue& operator=(event_queue&&) noexcept = default;

        ~event_queue() = default;

        /**
         * Push event data. The event is specified by explict event parameter.
         * @tparam E Event
         * @param d The event data
         * @return True if success
         */
        template <event E>
            requires helper_::template among_us<E>
        bool push(typename E::data_type&& d)
        {
            auto&& queue = get_queue<E>();

            return queue.push(std::forward<typename E::data_type>(d));
        }

        /**
         * Push event data.
         * The event is deduced automatically, but it will success only if there is only one event
         * type that has the data type.
         *
         * @tparam D Event data type
         * @param d data
         * @return True if success
         */
        template <typename D>
            requires helper_::template among_deduced_data<D> && (not helper_:: template among_us<D>)
        bool push(D&& d)
        {
            using event_type = typename helper_::template data_to_event<D>;

            auto&& queue = get_queue<event_type>();
            return queue.push(std::forward<D>(d));
        }

        /**
         * Get and pop out event data.
         * @tparam E Event Type
         * @return Event data if the queue related to the event type is not empty, nullopt otherwise.
         */
        template <event E>
            requires helper_::template among_us<E>
        auto pop() -> std::optional<event_data_t<E>>
        {
            using namespace boost::lockfree;
            auto&& queue = get_queue<E>();

            return std::move(queue.pop(uses_optional));
        }

        template <event E, typename Fn>
            requires (helper_::template invocable_with_event<Fn, E>)
        bool consume_one(Fn &&f)
        {
            auto &&queue = get_queue<E>();
            return queue.consume_one(std::forward<Fn>(f));
        }

        template <event E, typename Fn>
            requires helper_::template invocable_with_event<Fn, E>
        bool consume_all(Fn &&f)
        {
            auto &&queue = get_queue<E>();
            return queue.consume_all(std::forward<Fn>(f));
        }

        template <typename Fn>
            requires helper_::template deduced_invocable<Fn>
        bool consume_one(Fn &&f)
        {
            using event_type = typename helper_::template deduced_event_to_invoke<Fn>;
            auto &&queue = get_queue<event_type>();
            return queue.consume_one(std::forward<Fn>(f));
        }

        template <typename Fn>
            requires helper_::template deduced_invocable<Fn>
        bool consume_all(Fn &&f)
        {
            using event_type = typename helper_::template deduced_event_to_invoke<Fn>;
            auto &&queue = get_queue<event_type>();
            return queue.consume_all(std::forward<Fn>(f));
        }

        // TODO Tagged consume function.

    private:
        typename helper_::queue_map queue_map_{};
    };

}
