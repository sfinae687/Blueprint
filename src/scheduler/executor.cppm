//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//

/// @file
/// The abstraction of execute resource that defines the location of executing.

module;
#include <boost/log/trivial.hpp>
#include <boost/log/common.hpp>

#include <thread_pool/thread_pool.h>

#include <concepts>
#include <thread>

export module blueprint.scheduler:executor;
import :debug;

namespace blueprint::scheduler
{
    export template <std::size_t THREAD_COUNT, template<typename> typename FB>
    class thread_pool_executor
    {
        using FW = FB<void()>;

    public:
        thread_pool_executor()
            : pool_(THREAD_COUNT)
        {
            BOOST_LOG_SEV(sc_log, trace) << "Thread Pool Executor created with " << THREAD_COUNT << " threads";
        }
        thread_pool_executor(const thread_pool_executor &) = delete;
        thread_pool_executor& operator= (const thread_pool_executor &) = delete;

        template <typename Fn, typename... Arg>
            requires std::invocable<Fn, Arg...>
        void execute(Fn &&f, Arg&&... args)
        {
            pool_.enqueue_detach(std::forward<Fn>(f), std::forward<Arg>(args)...);
        }

    private:
        dp::thread_pool<FW, std::thread> pool_;
    };
}
