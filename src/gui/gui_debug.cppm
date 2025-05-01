//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-22.
//

module;
#include <stdexcept>
#include <string>

#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>

export module blueprint.gui:debug;

namespace blueprint::GUI
{
    using namespace std::string_literals;

    // debug flag

    #if not defined(NDEBUG) or defined(BP06_GUI_DEBUG)
        export constexpr bool gui_debug = true;
    #else
        export constexpr bool gui_debug = false;
    #endif

    // logger

    using logger_t = boost::log::sources::severity_logger<boost::log::trivial::severity_level>;
    inline logger_t gui_lg;

    // exceptions

    export class context_unexpected_state_exception : public std::logic_error
    {
    public:
        explicit context_unexpected_state_exception(const std::string &state)
            : logic_error("GUI context trap into unexpected state: "s + state)
        {

        }
    };

    export class context_unactive_exception final : public context_unexpected_state_exception
    {
    public:
        context_unactive_exception()
            : context_unexpected_state_exception("use a unactive context")
        {}
    };

}