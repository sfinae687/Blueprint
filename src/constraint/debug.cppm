//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-18.
//

module;

#include <boost/log/trivial.hpp>
#include <boost/log/common.hpp>

export module blueprint.constraint:debug;

namespace blueprint::constraint
{

#if not defined(NDEBUG) or not defined(NDEBUG_BLUEPRINT_CONSTRAINT)
    export constexpr bool constraint_debug = true;
#else
    export constexpr bool constraint_debug = false;
#endif

    using constraint_log_t = boost::log::sources::severity_channel_logger_mt<boost::log::trivial::severity_level>;

    auto constraint_log = []
    {
        using namespace boost::log;
        using namespace boost::log::attributes;
        constraint_log_t lg{};

        lg.add_attribute("Module", constant<std::string>("Constraint"));

        return lg;
    }();

}
