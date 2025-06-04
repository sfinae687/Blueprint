//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/2.
//

module;

#include <Eigen/Dense>
#include <opencv2/core.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/imgproc.hpp>

export module blueprint.builtin_node:image_kernel;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    export cv::Mat image_kernel(cv::Mat img, builtin_matrix_t kernel)
    {
        cv::Mat kel, rt;
        cv::eigen2cv(kernel, kel);
        cv::filter2D(img, rt, img.depth(), kel);
        return rt;
    }

    export constexpr char image_kernel_id[] = "core.image-process.kernel";
    export constexpr char image_kernel_name[] = "Kernel";
    export constexpr char image_kernel_description[] = "Scale the input image to the specify size.";

    export dyn_node::node_definition_proxy def_image_kernel_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_kernel,
            image_kernel_id, image_kernel_name, image_kernel_description
        );
    }

}