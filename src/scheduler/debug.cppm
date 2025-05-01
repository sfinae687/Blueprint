//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-1.
//

module;
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>

export module blueprint.scheduler:debug;

namespace blueprint::scheduler
{
    export using enum boost::log::trivial::severity_level;

    using scheduler_log_t = boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>;

    export inline scheduler_log_t sc_log;
}
