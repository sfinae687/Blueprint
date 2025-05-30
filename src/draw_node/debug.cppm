//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

module;
#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>

export module blueprint.draw_node:debug;

namespace blueprint::draw_node
{
    using enum boost::log::trivial::severity_level;

    using draw_node_logger_t =
        boost::log::sources::severity_logger<boost::log::trivial::severity_level>;
    export auto draw_node_logger = []
    {
        using namespace boost::log;
        draw_node_logger_t logger{};
        logger.add_attribute("Module", attributes::constant<std::string>("DrawNode"));
        return logger;
    }();

}
