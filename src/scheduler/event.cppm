//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-29.
//

module;
#include <concepts>
#include <boost/lockfree/queue.hpp>
#include <boost/mp11/mpl.hpp>
#include <boost/mp11/map.hpp>
#include <boost/hana.hpp>

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

        template <event E>
            requires helper_::template among_us<E>
        bool push(typename E::data_type&& d)
        {
            auto&& queue = get_queue<E>();

            return queue.push(std::forward<typename E::data_type>(d));
        }

        template <typename D>
            requires helper_::template among_deduced_data<D> && (not helper_:: template among_us<D>)
        bool push(D&& d)
        {
            using event_type = typename helper_::template data_to_event<D>;

            auto&& queue = get_queue<event_type>();
            return queue.push(std::forward<D>(d));
        }

        template <event E>
            requires helper_::template among_us<E>
        auto pop() -> std::optional<event_data_t<E>>
        {
            using namespace boost::lockfree;
            auto&& queue = get_queue<E>();

            return std::move(queue.pop(uses_optional));
        }

    private:
        typename helper_::queue_map queue_map_{};
    };

}
