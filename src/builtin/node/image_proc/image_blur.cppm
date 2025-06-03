//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/3.
//

module;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <utility>

export module blueprint.builtin_node:image_blur;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    export std::optional<cv::Mat> image_blur(cv::Mat img, int kz)
    {
        cv::Mat rt;
        cv::blur(img, rt, {kz, kz});
        return rt;
    }

    export constexpr char image_blur_id[] = "core.image-process.blur";
    export constexpr char image_blur_name[] = "Averaging Blur";
    export constexpr char image_blur_description[] = "Apply Averaging Blur with given kernel size.";

    export dyn_node::node_definition_proxy def_image_blur_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_blur,
            image_blur_id, image_blur_name, image_blur_description
        );
    }

    export std::optional<cv::Mat> image_gaussian_blur(cv::Mat img, int kz, float gamma)
    {
        if (kz % 2 != 1)
        {
            return std::nullopt;
        }
        cv::Mat rt;
        cv::GaussianBlur(img, rt, {kz, kz}, gamma);
        return rt;
    }

    export constexpr char image_gaussian_blur_id[] = "core.image-process.gaussian_blur";
    export constexpr char image_gaussian_blur_name[] = "Gaussian Blur";
    export constexpr char image_gaussian_blur_description[] = "Apply Gaussian Blur with given kernel size.";

    export dyn_node::node_definition_proxy def_image_gaussian_blur_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_gaussian_blur,
            image_gaussian_blur_id, image_gaussian_blur_name, image_gaussian_blur_description
        );
    }

}
