//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/4.
//

module;

#include <Eigen/Dense>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/eigen.hpp>

export module blueprint.builtin_node:to_image;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    export cv::Mat to_image(builtin_matrix_t mat)
    {
        cv::Mat rt;
        cv::eigen2cv(mat, rt);
        return rt;
    }

    export constexpr char to_image_id[] = "core.image-process.to-image";
    export constexpr char to_image_name[] = "To Image";
    export constexpr char to_image_description[] = "Dump a matrix to image";

    export dyn_node::node_definition_proxy def_matrix_to_img()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&to_image,
            to_image_id, to_image_name, to_image_description
        );
    }
}
