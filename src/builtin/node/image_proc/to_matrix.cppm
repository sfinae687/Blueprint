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

export module blueprint.builtin_node:to_matrix;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    export std::optional<builtin_matrix_t> to_matrix(cv::Mat img)
    {
        if (img.channels() != 1)
        {
            return std::nullopt;
        }

        builtin_matrix_t rt;
        cv::cv2eigen(img, rt);
        return rt;
    }

    export constexpr char to_matrix_id[] = "core.image-process.to-matrix";
    export constexpr char to_matrix_name[] = "To Matrix";
    export constexpr char to_matrix_description[] = "Dump a image to matrix";

    export dyn_node::node_definition_proxy def_image_to_matrix()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&to_matrix,
            to_matrix_id, to_matrix_name, to_matrix_description
        );
    }
}
