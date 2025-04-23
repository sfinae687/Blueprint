// Copyright (c) 2025 sfinea687
// Licensed under the MIT License. See LICENSE in the project root for details.

module;
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
module blueprint.gui;
import :debug;

namespace blueprint::GUI
{

    using enum boost::log::trivial::severity_level;

    // free function //

    bool init_gui()
    {
        const char *description;

        if (!glfwInit())
        {
            glfwGetError(&description);
            if (description)
            {
                BOOST_LOG_SEV(gui_lg, fatal) << "glfw init fail" << std::endl;
            }
            return false;
        }

        IMGUI_CHECKVERSION();
        return true;
    }

    void final_gui()
    {
        glfwTerminate();
    }

    // class window //

    window::window(window&& oth) noexcept
        : window_(oth.window_)
        , im_context_(oth.im_context_)
    {
        oth.im_context_ = nullptr;
        oth.window_ = nullptr;
    }

    window::window(std::string_view title, int w, int h)
        : window_(glfwCreateWindow(w, h, title.data(), nullptr, nullptr))
        , im_context_(ImGui::CreateContext())
    {
        glfwMakeContextCurrent(window_);

        auto &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init();

        glfwMakeContextCurrent(nullptr);
        glfwSwapInterval(1);

        ImGui::SetCurrentContext(nullptr);
        glfwMakeContextCurrent(nullptr);
    }

    window::draw_op_t window::set_draw_op(draw_op_t op) noexcept
    {
        using std::swap;
        swap(op, draw_op_);
        return op;
    }

    void window::render_loop()
    {
        auto gd = context_guard();
        while (! should_close())
        {
            prepare_drawing();
            draw_op_();
            finish_drawing();
        }
    }

    void window::redner()
    {
        auto gd = context_guard();
        prepare_drawing();
        draw_op_();
        finish_drawing();
    }

    std::pair<int, int> window::windows_size() const noexcept
    {
        int ww, wh;
        glfwGetWindowSize(window_, &ww, &wh);
        return {ww, wh};
    }

    bool window::should_close() const noexcept
    {
        return glfwWindowShouldClose(window_);
    }

    GLFWwindow* window::glfw_context() const noexcept
    {
        return window_;
    }

    ImGuiContext* window::imgui_context() const noexcept
    {
        return im_context_;
    }

    void window::prepare_drawing() const
    {
        switch (draw_stage_)
        {
        case to_prepare:
            glfwPollEvents();
            prepare_frame();
        case ready:
            return;
        case finished:
            BOOST_LOG_SEV(gui_lg, warning) << "prepare a drawing after it finished in the frame";
        }
    }

    void window::finish_drawing() const
    {
        adjust_viewport();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window_);
    }

    void window::adjust_viewport() const
    {
        int dw, dh;
        glfwGetFramebufferSize(window_, &dw, &dh);
        glViewport(0, 0, dw, dh);
    }

    window::~window() noexcept
    {
        assert(static_cast<bool>(window_) == static_cast<bool>(im_context_));
        if (im_context_)
        {
            release_context();
        }
    }

    window& window::operator=(window&& oth) noexcept
    {
        if (im_context_)
        {
            release_context();
        }

        window_ = oth.window_;
        im_context_ = oth.im_context_;
        oth.window_ = nullptr;
        oth.im_context_ = nullptr;

        return *this;
    }

    void window::prepare_frame() const
    {
        // precondition check.
        if constexpr (gui_debug)
        {
            if (!is_active())
            {
                throw context_unactive_exception();
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

    }


    void window::activate_context() const
    {
        glfwMakeContextCurrent(window_);
        ImGui::SetCurrentContext(im_context_);
    }

    void window::inactivate_context() const
    {
        if constexpr (gui_debug)
        {
            if (glfwGetCurrentContext() != window_ || ImGui::GetCurrentContext() != im_context_)
            {
                throw context_unexpected_state_exception("inactivate a unused context");
            }
        }
        glfwMakeContextCurrent(nullptr);
        ImGui::SetCurrentContext(nullptr);
    }

    void window::release_context()
    {
        if constexpr (gui_debug)
        {
            if (! window_ || ! im_context_)
            {
                throw context_unexpected_state_exception("the handle reference to no context");
            }
        }

        glfwMakeContextCurrent(window_);
        ImGui::SetCurrentContext(im_context_);

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(im_context_);
        glfwDestroyWindow(window_);

        window_ = nullptr;
        im_context_ = nullptr;
    }

    bool window::is_valid() const
    {
        return static_cast<bool>(window_) == static_cast<bool>(im_context_);
    }

    bool window::is_active() const
    {
        return window_
        && im_context_
        && glfwGetCurrentContext() == window_
        && ImGui::GetCurrentContext() == im_context_;
    }




}
