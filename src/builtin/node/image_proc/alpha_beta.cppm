//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/3.
//

module;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <utility>

export module blueprint.builtin_node:image_alpha_beta;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    export cv::Mat image_alpha_beta(cv::Mat img, float alpha, float beta)
    {
        cv::Mat rt;
        cv::convertScaleAbs(img, rt, alpha, beta);
        return rt;
    }

    export constexpr char image_alpha_beta_id[] = "core.image-process.alpha_beta";
    export constexpr char image_alpha_beta_name[] = "Linear transform";
    export constexpr char image_alpha_beta_description[] = "Apply a linear transform to a image";

    export dyn_node::node_definition_proxy def_image_alpha_beta_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_alpha_beta,
            image_alpha_beta_id, image_alpha_beta_name, image_alpha_beta_description
        );
    }

}
