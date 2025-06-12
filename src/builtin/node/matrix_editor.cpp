//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/5/31.
//

module;

#include <Eigen/Dense>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/eigen.hpp>

#include <proxy/proxy.h>

#include <memory>

module blueprint.builtin_node;
import :matrix_editor;

namespace blueprint::builtin
{

    // Matrix Editor

    id_type matrix_editor_def::id() const noexcept
    {
        return matrix_editor_id;
    }

    text_type matrix_editor_def::name() const noexcept
    {
        return "Matrix editor";
    }

    text_type matrix_editor_def::description() const noexcept
    {
        return "The node output a small matrix";
    }

    node_instance_proxy matrix_editor_def::create_node() noexcept
    {
        return std::make_unique<matrix_editor_node>();
    }

    node_instance_proxy matrix_editor_def::load(archive::input_archive_t& ar)
    {
        auto p = create_node();
        assert(p->type_id() == matrix_editor_id);

        using enum matrix_editor_node::mat_type;
        using size_context = matrix_editor_node::size_context;
        using gaussian_kernel_context = matrix_editor_node::gaussian_kernel_context;
        using eye_context = matrix_editor_node::eye_context;

        auto &&editor = proxy_cast<matrix_editor_node&>(*p);
        ar(editor.cur_item);

        switch (editor.cur_item)
        {
        case Custom:
            *editor.mat_ = load_matrix(ar);
            break;
        case Laplace_kernel:
        case Vertical_sobel:
        case Horizontal_sobel:
        case Vertical_Prewitt:
        case Horizontal_Prewitt:
            // No additional data to load
            break;
        case Gaussian_kernel:
        {
            auto &&gua = gaussian_kernel_context{};
            ar(gua.sz, gua.gamma);
            editor.arg_ = gua;
            break;
        }
        case Ones:
        case Zeros:
        {
            auto &&sz_ctx = size_context{};
            ar(sz_ctx.height, sz_ctx.width);
            editor.arg_ = sz_ctx;
            break;
        }
        case Eye:
        {
            auto &&sz = eye_context{};
            ar(sz.sz);
            editor.arg_ = sz;
            break;
        }
        default:
            break;
        }

        editor.flush_matrix();

        return p;
    }

    void matrix_editor_def::save(archive::output_archive_t& ar, node_instance_proxy& p)
    {
        assert(p->type_id() == matrix_editor_id);

        using enum matrix_editor_node::mat_type;
        using size_context = matrix_editor_node::size_context;
        using gaussian_kernel_context = matrix_editor_node::gaussian_kernel_context;
        using eye_context =  matrix_editor_node::eye_context;

        auto &&editor = proxy_cast<matrix_editor_node&>(*p);
        ar(editor.cur_item);

        switch (editor.cur_item)
        {
        case Custom:
            save_matrix(ar, *editor.mat_);
            break;
        case Laplace_kernel:
        case Vertical_sobel:
        case Horizontal_sobel:
        case Vertical_Prewitt:
        case Horizontal_Prewitt:
            // No additional data to save
            break;
        case Gaussian_kernel:
        {
            auto &&gua = std::get<gaussian_kernel_context>(editor.arg_);
            ar(gua.sz, gua.gamma);
            break;
        }
        case Ones:
        case Zeros:
        {
            auto &&sz_ctx = std::get<size_context>(editor.arg_);
            ar(sz_ctx.height, sz_ctx.width);
            break;
        }
        case Eye:
        {
            auto &&sz = std::get<eye_context>(editor.arg_);
            ar(sz.sz);
            break;
        }
        default:
            break;
        }
    }



    matrix_editor_node::matrix_editor_node()
        : mat_(std::make_shared<builtin_matrix_t>(0, 0))
    {}

    id_type matrix_editor_node::type_id() const noexcept
    {
        return matrix_editor_id;
    }
    std::span<const signature_t> matrix_editor_node::signatures() const noexcept
    {
        static signature_t sig {
            .input = {},
            .output = {matrix_id}
        };

        return {&sig, 1};
    }
    std::size_t matrix_editor_node::current_variant() const noexcept
    {
        return 0;
    }
    bool matrix_editor_node::set_variant(std::size_t) noexcept
    {
        return false;
    }
    bool matrix_editor_node::compute(data_sequence ds) noexcept
    {
        return ds.size() == 0;
    }
    data_sequence matrix_editor_node::output() const noexcept
    {
        return {mat_};
    }

    // Matrix Display

    void matrix_editor_node::flush_matrix()
    {
        switch (cur_item)
        {
        case Laplace_kernel:
            *mat_ = builtin_matrix_t{
                        {0, 1, 0},
                        {1, -4, 1},
                        {0, 1, 0},
                    };
            break;
        case Vertical_sobel:
            *mat_ = builtin_matrix_t {
                {1, 2, 1},
                {0, 0, 0},
                {-1, -2, -1},
            };
            break;
        case Horizontal_sobel:
            *mat_ = builtin_matrix_t {
                {1, 0, -1},
                {2, 0, -2},
                {1, 0, -1},
            };
            break;
        case Vertical_Prewitt:
            *mat_ = builtin_matrix_t{
                {1, 1, 1},
                {0, 0, 0},
                {-1, -1, -1},
            };
            break;
        case Horizontal_Prewitt:
            *mat_ = builtin_matrix_t{
                {1, 0, -1},
                {1, 0, -1},
                {1, 0, -1},
            };
            break;
        case Gaussian_kernel:
        {
            auto &&gua = std::get<gaussian_kernel_context>(arg_);
            auto kernel = cv::getGaussianKernel(gua.sz, gua.gamma, CV_64F);
            kernel = kernel * kernel.t();
            cv::cv2eigen(kernel, *mat_);
            break;
        }
        case Ones:
        {
            auto &&sz_ctx = std::get<size_context>(arg_);
            *mat_ = builtin_matrix_t::Ones(sz_ctx.height, sz_ctx.width);
            break;
        }
        case Zeros:
        {
            auto &&sz_ctx = std::get<size_context>(arg_);
            *mat_ = builtin_matrix_t::Zero(sz_ctx.height, sz_ctx.width);
            break;
        }
        case Eye:
        {
            auto &&sz = std::get<eye_context>(arg_);
            *mat_ = builtin_matrix_t::Identity(sz.sz, sz.sz);
            break;
        }
        default:
            break;
        }
    }

    id_type matrix_display_def::id() const noexcept
    {
        return matrix_display_id;
    }
    text_type matrix_display_def::name() const noexcept
    {
        return "Matrix Display";
    }
    text_type matrix_display_def::description() const noexcept
    {
        return "Display the matrix inputted";
    }

    node_instance_proxy matrix_display_def::create_node() noexcept
    {
        return std::make_unique<matrix_display_node>();
    }

    node_instance_proxy matrix_display_def::load(archive::input_archive_t&)
    {
        return create_node();
    }

    void matrix_display_def::save(archive::output_archive_t&, node_instance_proxy& p)
    {
        ;;
    }



    // instance

    id_type matrix_display_node::type_id() const noexcept
    {
        return matrix_display_id;
    }
    std::size_t matrix_display_node::current_variant() const noexcept
    {
        return 0;
    }
    bool matrix_display_node::set_variant(std::size_t) noexcept
    {
        return false;
    }
    std::span<const signature_t> matrix_display_node::signatures() const noexcept
    {
        static dyn_node::signature_t sig{
            .input = {matrix_id},
            .output =  {}
        };

        return {&sig, 1};
    }
    bool matrix_display_node::compute(data_sequence ds) noexcept
    {
        return ds.size() == 1;
    }
    data_sequence matrix_display_node::output() const noexcept
    {
        return {};
    }

} // namespace blueprint::builtin
