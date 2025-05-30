//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

module;
#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>

export module blueprint.flow:debug;

namespace blueprint::flow
{
    export using enum boost::log::trivial::severity_level;

    using flow_logger_t = boost::log::sources::severity_logger<boost::log::trivial::severity_level>;

    export auto flow_logger = []
    {
        using namespace boost::log;
        flow_logger_t logger{};
        logger.add_attribute("Module", attributes::constant<std::string>("Flow"));

        return logger;
    }();
}
