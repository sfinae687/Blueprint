//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-28.
//

module;
#include <GLFW/glfw3.h>
#include <imgui.h>

#include <boost/scope/scope_exit.hpp>

#include <functional>
#include <string_view>

export module blueprint.gui:window;
import :forward_declaration;

namespace blueprint::GUI
{

    // root gui context window.


    /**
     * The class manages a window context. The most member function of the class could just invoke when the context is
     * current context.
     */
    export class window
    {
    public:

        using draw_op_t = std::move_only_function<void(void)>;
        using update_op_t = std::move_only_function<void(void)>;

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
         * Set the operation will be run before drawing to update logic.
         * @return The operation set previously.
         */
        update_op_t set_update_operation(update_op_t) noexcept;

        /**
         * Set the operation will be run in render loop.
         *
         * @return The original draw operation. It would be empty if the operation hasn't been set.
         */
        draw_op_t set_draw_operation(draw_op_t) noexcept;

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
        void entry(std::invocable<> auto&& fn);

        // information

        /**
         * The size of window measured by pixel.
         * @return Width as first, height as second.
         */
        [[nodiscard]] std::pair<int, int> frame_buff_size() const;

        /**
         * test whether windows should finish.
         *
         * @return true if windows is requested to close
         */
        [[nodiscard]] bool should_close() const noexcept;

        // underlying handle

        /**
         * Get the underlying context of glfw library.
         *
         * @return the glfw context hold by the object.
         */
        [[nodiscard]] GLFWwindow* glfw_context() const noexcept;

        /**
         * Get the underlying context of imgui.
         *
         * @return the imgui context hold by the object.
         */
        [[nodiscard]] ImGuiContext* imgui_context() const noexcept;


        // draw stage operation

        /**
         * Execute the full render operation once.
         *
         * The full render operation includes: logic update, and drawing.
         *
         * @pre The context is set and the drawing stage is at 'to_prepare'.
         */
        void do_render();

        /**
         * Update logic, including event polling, viewport adjusting and user defined update operation.
         *
         * @pre The current context is set, and drawing stage doesn't reach updated.
         * @post The drawing stage will process to 'updated'.
         */
        void update();

        /**
         * Config context for drawing. You may call the function many times in a frame, but only the
         * first call is affect.
         *
         * The operation doesn't execute full-update operation although the drawing stage doesn't reach it.
         * If it occurs, the minimal operation to push the drawing stage.
         *
         * @pre context is active and the drawing stage isn't reach 'finished'.
         * @post could begin to call the function to drawing the screen.
         */
        void prepare_drawing();

        /**
         * Finish drawing, adjust viewport and update buffer.
         *
         * @pre context is active.
         * @post The window has been updated with drawn content. The drawing stage will back to_prepare.
         */
        void finish_drawing();

        // Public context operation

        /**
         * Add an external context. The window doesn't have the ownship of the context. The external context will be
         * activated when the window context activate.
         */
        void add_external_context(context_manage_proxy);

    private:

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


        enum draw_stage_t
        {
            to_prepare,
            updated,
            ready,
            finished,
        };

        // auxiliary operations

        [[nodiscard]] bool is_valid() const;

        [[nodiscard]] bool is_active() const;


        // context

        GLFWwindow *window_ = nullptr;
        ImGuiContext *im_context_ = nullptr;
        std::vector<context_manage_proxy> extension_manage_{};

        // internal info

        draw_op_t draw_op_{};
        update_op_t update_op_{};
        draw_stage_t draw_stage_ = to_prepare;



    };

    // template function for window

    inline auto window::context_guard() const
    {
        activate_context();
        return boost::scope::scope_exit([&]{inactivate_context();});
    }

    void window::entry(std::invocable<> auto&& fn)
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