//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-1.
//

/// @file
/// Poller provides a component to present the basic action in a polling loop.

module;
#include <boost/hana/map.hpp>
#include <boost/hana.hpp>
#include <boost/mp11/mpl.hpp>
#include <boost/log/common.hpp>

#include <functional>
#include <iomanip>

export module blueprint.scheduler:poller;
import :event;
import :debug;

namespace blueprint::scheduler
{

    template <typename T>
    concept poller = requires (T t)
    {
        t.poll_once();
        t.poll_nowait();
    };

    template <typename T>
    concept controllable_poller = poller<T> && requires (T t)
    {
        t.poll_loop();
        t.finish_loop();
    };

    export template <event... ES>
    class event_poller
    {
        using helper_ = details::event_queue_helper<ES...>;
        using queue_type = event_queue<ES...>;

        template <event E>
            requires helper_::template among_us<E>
        using poly_fn = std::function<void(event_data_t<E>)>;

        static auto event_type_tuple()
        {
            using namespace boost::mp11;
            namespace hana = boost::hana;
            using event_tuple_t = mp_rename<
                mp_transform<hana::type, typename helper_::event_type_list>,
                hana::tuple
            >;
            return event_tuple_t{};
        }

        static auto call_back_map()
        {
            using namespace boost::mp11;
            namespace hana = boost::hana;
            using event_tuple_t = mp_rename<
                mp_transform<hana::type, typename helper_::event_type_list>,
                hana::tuple
            >;

            event_tuple_t event_tuple{};
            return hana::to_map(hana::transform(event_tuple, [](auto e)
            {
                return hana::make_pair(e, poly_fn<typename decltype(+e)::type>{});
            }));
        }

        template <event E>
            requires helper_::template among_us<E>
        auto&& cb_slot()
        {
            namespace hana = boost::hana;
            return cb_[hana::type_c<E>];
        }

        template <event E>
            requires helper_::template among_us<E>
        bool poll_event(boost::hana::basic_type<E> e)
        {
            return queue_.template consume_one<E>([&](event_data_t<E> d)
            {
                auto &&cb = cb_[e];
                if (cb)
                {
                    cb(std::move(d));
                }
            });
        }

    public:
        explicit event_poller(queue_type &q)
            : queue_(q)
            , cb_(call_back_map())
        {

        }

        event_poller(const event_poller &) = delete;
        event_poller& operator= (const event_poller &) = delete;

        template <event E, typename Fn>
            requires helper_::template invocable_with_event<Fn, E>
        void set_callback(Fn &&f)
        {
            BOOST_LOG_SEV(sc_log, trace) << "Event poller callback for type "
                << std::quoted(typeid(E).name()) << "is set";
            cb_slot<E>() = std::forward<Fn>(f);
        }

        template <typename Fn>
            requires helper_::template deduced_invocable<Fn>
        void set_callback(Fn &&f)
        {
            using event_type = typename helper_::template deduced_event_to_invoke<Fn>;
            set_callback<event_type, Fn>(std::forward<Fn>(f));
        }

        template <event E, typename Fn>
            requires helper_::template invocable_with_event<Fn, E>
        void chain_callback(Fn&& f)
        {
            if (auto&& cb = cb_slot<E>())
            {
                set_callback<E>([ncb = std::forward<Fn>(f), cb = std::move(cb)] (event_data_t<E> d)
                {
                    ncb(d);
                    cb(d);
                });
            } else
            {
                set_callback<E>(std::forward<Fn>(f));
            }
        }

        void poll_once()
        {
            namespace hana = boost::hana;
            hana::unpack(event_type_tuple(), [this](auto... es)
            {
                (... || poll_event<typename decltype(+es)::type>(es));
            });
        }
        void poll_nowait()
        {
            namespace hana = boost::hana;
            hana::for_each(event_type_tuple(), [&](auto e)
            {
                using event_type = typename decltype(+e)::type;
                queue_.template consume_all<event_type>([&](event_data_t<event_type> d)
                {
                    auto &&f = cb_[e];
                    if (f)
                    {
                        f(std::move(d));
                    }
                });
            });
        }

    private:
        queue_type &queue_;
        decltype(call_back_map()) cb_;
    };

}
