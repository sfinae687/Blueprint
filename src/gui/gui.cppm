// Copyright (c) 2025 sfinea687
// Licensed under the MIT License. See LICENSE in the project root for details.

/// @file
/// A naive encapsulation of ImGUI with glfw and OpenGL3 as backend.

module;
#include <imgui.h>
#include <GLFW/glfw3.h>

#include <string_view>
#include <utility>
#include <functional>

#include <boost/scope/scope_exit.hpp>

export module blueprint.gui;
export import :debug;

namespace blueprint::GUI
{

    // GUI and Window

    /**
     * Execute the global scope initialization for gui.
     * @return true if success.
     */
    export bool init_gui();

    /**
     * Call the function after all gui operation finished.
     */
    export void final_gui();

    /**
     * The class manages a window context. The most member function of the class could just invoke when the context is
     * current context.
     */
    export class window
    {
    public:

        using draw_op_t = std::move_only_function<void(void)>;

        // copy control: movable, default constructible

        window() = default;
        window(window &&) noexcept;
        ~window() noexcept;

        window& operator= (window &&) noexcept;

        // constructor

        /**
         * create the context with specify size and title .
         *
         * @param title title of the window
         * @param w width
         * @param h heigh
         */
        window(std::string_view title, int w, int h);

        // render

        /**
         * Set the operation will be run in render loop.
         *
         * @return The original draw operation. It would be empty if the operation hasn't been set.
         */
        draw_op_t set_draw_op(draw_op_t) noexcept;

        /**
         * Lunch a loop for render. It would quit when the window should be close or the drawing operation throw
         * exception.
         *
         */
        void render_loop();

        /**
         * Lunch render operation once.
         */
        void redner();

        // scope operation

        /**
         * Take an invocable as the argument and run it in the context.
         *
         * @pre the current context should be this object, or NULL.
         *
         * @param fn the operation should take in the context.
         */
        void entry(std::invocable<void> auto &&fn);

        // information

        /**
         * get the size of the native windows.
         *
         * @return width as first, height as second.
         */
        std::pair<int, int> windows_size() const noexcept;

        /**
         * test whether windows should finish.
         *
         * @return true if windows is requested to close
         */
        bool should_close() const noexcept;

        // underlying handle

        /**
         * Get the underlying context of glfw library.
         *
         * @return the glfw context hold by the object.
         */
        GLFWwindow* glfw_context() const noexcept;

        /**
         * Get the underlying context of imgui.
         *
         * @return the imgui context hold by the object.
         */
        ImGuiContext* imgui_context() const noexcept;

    protected:

        // draw stage operation

        /**
         * Config context for drawing. You may call the function many times in a frame, but only the
         * first call is affect.
         *
         * @pre context is active and the drawing stage isn't reach 'finished'.
         * @post could begin to call the function to drawing the screen.
         */
        void prepare_drawing() const;

        /**
         * finish drawing, adjust viewport and update buffer.
         *
         * @pre context is active.
         * @post The window has been updated with drawn content. The drawing stage will back to_prepare.
         */
        void finish_drawing() const;

        // aux

        /**
         * adjust underlying viewport to match the windows size.
         *
         * @pre context is active.
         */
        void adjust_viewport() const;


        /**
         * prepare a new frame.
         *
         * @pre context is active and the drawing stage isn't reach ready.
         * @post be able to draw.
         */
        void prepare_frame() const;

        // context operation

        /**
         * Active the gui context the object holds.
         *
         * If there is another context has been activated, activating a context will override the original context.
         * But it does not cause any server problem normally.
         *
         */
        void activate_context() const;

        /**
         * Inactive the gui context the object holds.
         *
         * @pre The context is activated now.
         */
        void inactivate_context() const;

        /**
         * Activate context and construct a guard to inactivated it.
         * @return A guard that close the context.
         */
        [[nodiscard]] auto context_guard() const;

        /**
         * Release the resource held by the object, and clean the inner state.
         *
         * @pre The context is NOT activated now.
         */
        void release_context();

    private:

        enum draw_stage_t
        {
            to_prepare,
            ready,
            finished,
        };

        // auxiliary operations

        [[nodiscard]] bool is_valid() const;

        [[nodiscard]] bool is_active() const;


        // context

        GLFWwindow *window_ = nullptr;
        ImGuiContext *im_context_ = nullptr;

        // internal info

        draw_op_t draw_op_{};
        draw_stage_t draw_stage_ = to_prepare;



    };

    // template function for window

    inline auto window::context_guard() const
    {
        activate_context();
        return boost::scope::scope_exit([&]{inactivate_context();});
    }

    void window::entry(std::invocable<void> auto&& fn)
    {
        if (is_active())
        {
            fn();
        }
        else
        {
            auto context_gd = context_guard();
            prepare_drawing();
            fn();
        }
    }

}