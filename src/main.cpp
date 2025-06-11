// Copyright (c) 2025 sfinea687
// Licensed under the MIT License. See LICENSE in the project root for details.

import application;

#include <imgui.h>
#include <nfd.h>

#include <boost/log/utility/setup.hpp>

#include <iostream>

void init_log()
{
    using namespace boost::log;
    add_common_attributes();
    add_console_log(
        std::cout,
        keywords::format = "[%TimeStamp%] [%ThreadID%] [%Module%] [%Severity%] %Message%"
    );
}

int main(int argc, char *argv[])
{
    using namespace application;

    NFD_Init();
    init_log();

    auto app_gd = application::app_guard();

    application_context app{};

    return app.run();
}
