//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-24.
//

/// @file
/// Provide some function calling ImGui::Begin with predefined config set.

module;
#include <Eigen/Dense>
#include <opencv2/core.hpp>
#include <imgui.h>

#include <boost/hana.hpp>

#include <concepts>
#include <ranges>
#include <string_view>
#include <vector>

#include "../../lib/glfw/src/internal.h"

export module blueprint.gui:begin;

namespace blueprint::GUI
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    namespace config
    {
        constexpr std::size_t input_number_width = 64;
    }


    // --- Window ---//

    /**
     * Begin a window fill viewport, and keep staying in the background.
     */
    export bool begin_main_window(std::string_view title, bool* p_open) noexcept;

    namespace details
    {
        using namespace boost::hana;
        constexpr auto imgui_data_type_map = make_map(
            make_pair(type_c<std::int8_t>, ImGuiDataType_S8),
            make_pair(type_c<std::int16_t>, ImGuiDataType_S16),
            make_pair(type_c<std::int32_t>, ImGuiDataType_S32),
            make_pair(type_c<std::int64_t>, ImGuiDataType_S64),
            make_pair(type_c<std::uint8_t>, ImGuiDataType_U8),
            make_pair(type_c<std::uint16_t>, ImGuiDataType_U16),
            make_pair(type_c<std::uint32_t>, ImGuiDataType_U32),
            make_pair(type_c<std::uint64_t>, ImGuiDataType_U64),
            make_pair(type_c<char*>, ImGuiDataType_String),
            make_pair(type_c<float>, ImGuiDataType_Float),
            make_pair(type_c<double>, ImGuiDataType_Double)
        );

        template <typename T>
        consteval auto get_imgui_data_type_enum()
        {
            using data_type = std::remove_cvref_t<T>;
            constexpr auto ans = find(imgui_data_type_map, type_c<data_type>);
            if constexpr (is_just(ans))
            {
                return ans.value();
            }
            else
            {
                return -1;
            }
        }

        static_assert(get_imgui_data_type_enum<double>() == ImGuiDataType_Double);
    }

    // --- Input --- //

    export template <typename T>
        requires std::integral<T> || std::floating_point<T>
    constexpr auto imgui_data_type = details::get_imgui_data_type_enum<T>();

    export template <typename T>
        requires (imgui_data_type<T> != -1)
    bool input_number(std::string_view label, T &value)
    {
        ImGui::SetNextItemWidth(config::input_number_width);
        return ImGui::InputScalar(label.data(), imgui_data_type<T>, &value);
    }

    export template <typename T, std::size_t N>
        requires (imgui_data_type<T> != -1)
    bool input_number(std::string_view label, T value[N])
    {
        ImGui::SetNextItemWidth(config::input_number_width * N);
        return ImGui::InputScalarN(label.data(), imgui_data_type<T>, value, N);
    }

    export template <typename T>
        requires (imgui_data_type<T> != -1)
    bool input_number(std::string_view label, std::size_t n, T *value)
    {
        ImGui::SetNextItemWidth(config::input_number_width * n);
        return ImGui::InputScalarN(label.data(), imgui_data_type<T>, value, n);
    }


    export template <typename T, int r, int c>
    bool matrix_editor(std::string_view label, Eigen::Matrix<T, r, c> &mat) noexcept
    {
        bool changed_flag = false;

        std::size_t rows = mat.rows();
        std::size_t cols = mat.cols();

        static_assert(std::same_as<std::size_t, std::uint64_t>);

        auto row_lab = std::format("col##{}", label);
        auto col_lab = std::format("row##{}", label);
        bool row_change = input_number(row_lab, rows);
        ImGui::SameLine();
        auto col_change = input_number(col_lab, cols);
        if (row_change || col_change)
        {
            changed_flag = true;
            mat.conservativeResize(rows, cols);
        }

        if (rows > 10 || cols > 10)
        {
            ImGui::Text("The matrix is too large to edit by gui");
            return changed_flag;
        }

        if (rows > 0 && cols > 0) for (std::size_t i=0; i<rows; ++i)
        {
            auto &&row = mat.row(i);
            auto row_data = ranges::to<std::vector>(row);
            auto row_label = std::format("##{}##r{}", label, i);
            if (input_number(row_label, row_data.size(), row_data.data()))
            {
                changed_flag = true;
                std::copy(row_data.begin(), row_data.end(), row.begin());
            }
        }

        return changed_flag;
    }

    export template <typename T, int r, int c>
    bool matrix_editor(std::string_view label, const Eigen::Matrix<T, r, c> &mat) noexcept
    {
        auto rows = mat.rows();
        auto cols = mat.cols();

        auto size_text = std::format("Size:{} x {}", rows, cols);

        Eigen::IOFormat mat_format{
            5,
        };

        std::stringstream str_in;
        str_in << mat.format(mat_format);
        std::string str = str_in.str();
        ImGui::Text("%s", str.c_str());

        return false;
    }

    // --- Image --- //

    export ImTextureID load_opencv_image(const cv::Mat& image);
    export void unload_texture(ImTextureID);

    /**
     * class image manages the resource to render an image to GUI.
     */
    export class image
    {
    public:
        enum class image_type
        {
            none,
            rgba,
            gray,
        };

        image() = default;
        image(const image &) = delete;
        image(image &&) noexcept;
        ~image();
        image& operator= (const image &) = delete;
        image& operator= (image &&) noexcept;

        image(ImTextureID id, image_type ty, ImVec2 sz);
        explicit image(const cv::Mat &);

        explicit operator bool() const;

        image& operator= (const cv::Mat &);

        [[nodiscard]] image_type type() const;
        [[nodiscard]] ImTextureID id() const;

        [[nodiscard]] std::size_t width() const;
        [[nodiscard]] std::size_t height() const;

        void show(float scale=1.0) const;

    private:

        cv::Mat dump_image() const;
        void release();
        void bind_swizzle() const;

        image_type ty_ = image_type::none;
        GLuint tid_;

        std::size_t width_;
        std::size_t height_;
    };
}