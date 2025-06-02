//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/2.
//

module;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

export module blueprint.builtin_node:image_brightness;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    export cv::Mat image_brightness(cv::Mat img, float beta)
    {
        cv::Mat rt;
        cv::convertScaleAbs(img, rt, 1, beta);
        return rt;
    }

    export constexpr char image_brightness_id[] = "core.image-process.brightness";
    export constexpr char image_brightness_name[] = "Image brightness";
    export constexpr char image_brightness_description[] = "Scale the input image to the specify size.";

    export dyn_node::node_definition_proxy def_image_brightness_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_brightness,
            image_brightness_id, image_brightness_name, image_brightness_description
        );
    }

}
