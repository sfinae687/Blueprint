//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/4.
//

module;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

export module blueprint.builtin_node:image_canny;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{


    cv::Mat image_canny(cv::Mat img, double f1, double f2)
    {
        cv::Mat u8_img;
        img.convertTo(u8_img, CV_8U, 255.);

        cv::Mat rt;
        cv::Canny(u8_img, rt, f1, f2);

        rt.convertTo(rt, CV_32F, 1/255.);
        return rt;
    }

    export constexpr char image_canny_id[] = "core.image-process.canny";
    export constexpr char image_canny_name[] = "Canny";
    export constexpr char image_canny_description[] = "Border detect";

    export dyn_node::node_definition_proxy def_image_canny_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_canny,
            image_canny_id, image_canny_name, image_canny_description
        );
    }
}
