//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/11.
//

module;

#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>

#include <imgui.h>

module application;

namespace application
{
    application_context::application_context()
        : gui_("Blueprint Node Editor", 1080, 720)
    {
        setup_logger();
        editors_ = new blueprint_application(gui_);

        gui_.set_update_operation([this] {update();});
        gui_.set_draw_operation([this] {draw();});
    }

    application_context::~application_context()
    {
        if (editors_)
        {
            delete editors_;
        }
    }

    int application_context::run()
    {
        gui_.render_loop();
        return 0;
    }

    void application_context::setup_logger()
    {
        using namespace boost::log;
        logger.add_attribute("Module", attributes::constant<std::string>("Application"));
    }

    void application_context::update()
    {
        if (editors_)
        {
            editors_->update();
        }
    }

    void application_context::draw()
    {
        using namespace GUI;
        if (begin_main_window("main", &main_open) && editors_)
        {
            editors_->draw();
        }
        ImGui::End();
    }






}