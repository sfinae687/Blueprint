// Copyright (c) 2025 sfinea687
// Licensed under the MIT License. See LICENSE in the project root for details.

import blueprint;

#include "imgui.h"

#include <boost/log/utility/setup.hpp>

#include <iostream>

void init_log()
{
    using namespace boost::log;
    add_common_attributes();
    add_console_log(
        std::clog,
        keywords::format = "[%TimeStamp%] [%ThreadID%] [%Module%] [%Severity%] %Message%"
    );
}

int main(int argc, char *argv[])
{

    init_log();

    auto app_gd = blueprint::app_guard();

    blueprint::blueprint_application app{};

    return app.run();
}
