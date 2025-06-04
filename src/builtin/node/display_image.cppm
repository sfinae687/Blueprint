//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//

module;

#include <boost/log/trivial.hpp>

#include <opencv2/core.hpp>
#include <proxy/proxy.h>
#include <iostream>

export module blueprint.builtin_node:display_image;
import blueprint.dyn_node;
import blueprint.stk_node;
import blueprint.gui;
import :image;

namespace blueprint::builtin
{
    using stk_node::template_definition;
    using dyn_node::data_sequence;
    using dyn_node::signature_t;

    signature_t display_image_signature {
        .input = {image_id},
        .output = {},
    };

    export constexpr char display_image_id[] = "core.display-image";
    constexpr char display_image_name[] = "Display image";
    constexpr char display_image_description[] = "Display a image inputted";

    export class display_image_instance
    {
    public:
        id_type type_id() const noexcept
        {
            return display_image_id;
        }

        [[nodiscard]] std::span<const signature_t> signatures() const noexcept
        {
            return {&display_image_signature, 1};
        }
        [[nodiscard]] std::size_t current_variant() const noexcept
        {
            return 0;
        }
        [[nodiscard]] bool set_variant(std::size_t) const noexcept
        {
            return false;
        }
        [[nodiscard("Check the state of compute is always neccessary")]]
        bool compute(data_sequence ds) noexcept
        {
            if (ds.size() != 1)
            {
                return false;
            }

            origin_ = proxy_cast<cv::Mat&>(*ds[0]);
            set_flush();
            return true;
        }
        [[nodiscard]] data_sequence output() const noexcept
        {
            return {};
        }

        [[nodiscard]] bool has_image() const
        {
            return ! origin_.empty() || static_cast<bool>(img_);
        }

        [[nodiscard]] std::size_t width() const
        {
            return img_.width();
        }

        [[nodiscard]] std::size_t height() const
        {
            return img_.height();
        }

        [[nodiscard]] bool need_flush() const
        {
            return flush_needed_;
        }

        void set_flush()
        {
            flush_needed_ = true;
        }

        void flush_image()
        {
            if (need_flush())
            {
                cv::Mat display_image = origin_.clone();

                if (normalize)
                {
                    cv::normalize(display_image, display_image, 0, 1, cv::NORM_MINMAX, CV_64F);
                }

                assert(origin_.depth() == CV_64F);
                cv::convertScaleAbs(display_image, display_image, alpha*255.0, beta);

                img_ = display_image;
                unset_flush();

            }
        }

        bool display() {
            if (img_)
            {
                img_.show(scale);
                return true;
            }
            return false;
        }

    private:

        void unset_flush()
        {
            flush_needed_ = false;
        }

        bool flush_needed_ = false;

        cv::Mat origin_;
        GUI::image img_{};

    public:
        // process before display

        bool normalize = false;
        float alpha = 1.0;
        float beta = 0.0;

        float scale = 1.0;

    };


    export using display_image_definition = template_definition<
        display_image_id,
        display_image_name, display_image_description,
        display_image_instance
    >;
}