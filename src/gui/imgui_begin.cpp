//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-24.
//

module;
#include <imgui.h>
#include <opencv2/core.hpp>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <string_view>

#include <iostream>

module blueprint.gui;
import :begin;

namespace blueprint::GUI
{
    bool begin_main_window(std::string_view title, bool* p_open) noexcept
    {
        const auto vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(vp->WorkSize);

        constexpr auto main_win_flag = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
        return ImGui::Begin(title.data(), p_open, main_win_flag);
    }

    void upload_grayscale_image(const cv::Mat &image)
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            image.cols, image.rows, 0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            image.data
        );
    }

    void upload_rgba_image(const cv::Mat &image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    }

    ImTextureID load_opencv_image(const cv::Mat& image)
    {

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

        if (image.channels() == 1)
        {
            upload_grayscale_image(image);
        }
        else if (image.channels() == 4)
        {
            upload_rgba_image(image);
        }
        else
        {
            assert(false);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }
    void unload_texture(ImTextureID id)
    {
        auto gl_id = static_cast<GLuint>(id);
        glDeleteTextures(1, &gl_id);
    }

    // image
    namespace
    {
        GLint gray_swizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
    }

    image::image_type deduced_image_type(const cv::Mat &mat)
    {
        using enum image::image_type;
        switch (mat.channels())
        {
        case 1:
            return gray;
        case 4:
            return rgba;
        default:
            return none;
        }
    }

    image::image(image&& that) noexcept : ty_(that.ty_), tid_(that.tid_), width_(that.width_), height_(that.height_)
    {
        that.ty_ = image_type::none;
    }

    image::~image()
    {
        if (ty_ != image_type::none)
        {
            release();
        }
    }

    ImTextureID image::id() const { return tid_; }
    std::size_t image::width() const { return width_; }
    std::size_t image::height() const
    {
        return height_;
    }
    image::image_type image::type() const
    {
        return ty_;
    }

    image& image::operator=(image&& that) noexcept
    {
        if (ty_ != image_type::none)
        {
            release();
        }
        ty_ = that.ty_;
        tid_ = that.tid_;
        width_ = that.width_;
        height_ = that.height_;
        that.ty_ = image_type::none;
        return *this;
    }
    image::image(ImTextureID id, image_type ty, ImVec2 sz) : ty_(ty), tid_(id), height_(sz.x), width_(sz.y) {}

    image::image(const cv::Mat& mat) :
        ty_(deduced_image_type(mat)), tid_(load_opencv_image(mat)), width_(mat.cols), height_(mat.rows)
    {
        assert(ty_ != image_type::none);
        bind_swizzle();
    }
    image::operator bool() const
    {
        return ty_ != image_type::none;
    }

    image& image::operator=(const cv::Mat& mat)
    {
        if (ty_ != image_type::none)
        {
            release();
        }
        ty_ = deduced_image_type(mat);
        tid_ = load_opencv_image(mat);
        width_ = mat.cols;
        height_ = mat.rows;
        assert(ty_ != image_type::none);
        bind_swizzle();
        return *this;
    }

    void image::show(float scale) const
    {
        auto im_tid = id();
        ImVec2 sz(width_, height_);
        sz.x *= scale;
        sz.y *= scale;
        ImGui::Image(im_tid, sz);
    }


    void image::release()
    {
        glDeleteTextures(1, &tid_);
        ty_ = image_type::none;
    }
    void image::bind_swizzle() const
    {
        using enum image_type;
        GLint *param;

        switch (ty_)
        {
        case gray:
            param = gray_swizzle;
            break;
        default:
            return;
        }

        glBindTexture(GL_TEXTURE_2D, tid_);
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, param);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

} // namespace blueprint::GUI
