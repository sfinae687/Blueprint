//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/2.
//

module;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

export module blueprint.builtin_node:image_resize;
import :builtin_mapper;
import blueprint.dyn_node;
import blueprint.stk_node;

namespace blueprint::builtin
{
    export cv::Mat image_resize(cv::Mat img, std::size_t width, std::size_t height)
    {
        cv::Mat rt;
        cv::resize(img, rt, cv::Size(width, height));
        return rt;
    }

    export constexpr char image_resize_id[] = "core.image-process.resize";
    export constexpr char image_resize_name[] = "Image resize";
    export constexpr char image_resize_description[] = "Scale the input image to the specify size.";

    export dyn_node::node_definition_proxy def_image_resize_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_resize,
            image_resize_id, image_resize_name, image_resize_description
        );
    }


}
