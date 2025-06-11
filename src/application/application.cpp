//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/11.
//

module;

#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>

#include <imgui.h>
#include <nfd.h>

#include <fstream>

module application;
import blueprint;
import blueprint.archive;

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

    void application_context::save_editor()
    {
        assert(editors_);

        nfdu8char_t *out_path;
        nfdu8filteritem_t filter[] = {
            {"Archive", "bp06"}
        };
        nfdsavedialogu8args_t arg = {0};
        arg.filterList = filter;
        arg.filterCount = std::size(filter);

        nfdresult_t dialog_result = NFD_SaveDialogU8_With(&out_path, &arg);
        if (dialog_result == NFD_OKAY)
        {
            std::ofstream out(out_path, std::ios::binary | std::ios::trunc);
            archive::output_archive_t ar(out);
            editors_->save(ar);
        }
    }

    void application_context::load_editor()
    {
        assert(editors_);

        nfdu8char_t *out_path;
        nfdu8filteritem_t filter[] = {
            {"Archive", "bp06"}
        };
        nfdopendialogu8args_t arg = {0};
        arg.filterList = filter;
        arg.filterCount = std::size(filter);

        nfdresult_t dialog_result = NFD_OpenDialogU8_With(&out_path, &arg);
        if (dialog_result == NFD_OKAY)
        {
            delete editors_;
            std::ifstream in(out_path, std::ios::binary);
            archive::input_archive_t ar(in);
            editors_ = new blueprint_application(gui_, ar);
        }
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
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Save"))
                    {
                        save_editor();
                    }

                    if (ImGui::MenuItem("Load"))
                    {
                        load_editor();
                    }

                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            editors_->draw();
        }
        ImGui::End();
    }

}