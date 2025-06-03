//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/3.
//

module;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

export module blueprint.builtin_node:image_grayscale;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    export std::optional<cv::Mat> image_grayscale(cv::Mat img)
    {
        if (img.channels() == 4)
        {
            cv::Mat ans;
            cv::cvtColor(img, ans, cv::COLOR_RGBA2GRAY);
            return ans;
        }
        else if (img.channels() == 1)
        {
            return img.clone();
        }
        else
        {
            return std::nullopt;
        }
    }

    export constexpr char image_grayscale_id[] = "core.image-process.grayscale";
    export constexpr char image_grayscale_name[] = "Gray scale";
    export constexpr char image_grayscale_description[] = "Transform a image into grayscale.";

    export dyn_node::node_definition_proxy def_image_grayscale_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_grayscale,
            image_grayscale_id, image_grayscale_name, image_grayscale_description
        );
    }

}
