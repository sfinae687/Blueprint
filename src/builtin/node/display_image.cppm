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
            return true;
        }
        [[nodiscard]] data_sequence output() const noexcept
        {
            return {};
        }

        void flush_image()
        {
            if (! origin_.empty())
            {
                img_ = origin_;
                origin_.release();
            }
        }

        bool display() {
            if (img_)
            {
                img_.show();
                return true;
            }
            return false;
        }

    private:
        cv::Mat origin_;
        GUI::image img_;

    };


    export using display_image_definition = template_definition<
        display_image_id,
        display_image_name, display_image_description,
        display_image_instance
    >;
}