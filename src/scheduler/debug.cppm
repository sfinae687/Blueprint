//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-1.
//

module;
#include <boost/log/trivial.hpp>
#include <boost/log/common.hpp>
#include <boost/log/attributes.hpp>

export module blueprint.scheduler:debug;

namespace blueprint::scheduler
{

    // flag

#if defined(NDEBUG) || defined(SCHEDULER_NO_DEBUG)
    constexpr bool scheduler_debug = false;
#else
    constexpr bool scheduler_debug = true;
#endif

    // logger

    export using enum boost::log::trivial::severity_level;

    using scheduler_log_t = boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>;

    export inline auto sc_log = []
    {
        using namespace boost::log;
        scheduler_log_t logger{};
        logger.add_attribute("Module", attributes::constant<std::string>("scheduler"));
        return logger;
    }();

}
