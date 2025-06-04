//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/3.
//

module;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

export module blueprint.builtin_node:image_weighted;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    export std::optional<cv::Mat> image_weighted(float alpha, cv::Mat lhs, float beta, cv::Mat rhs)
    {
        if (lhs.channels() != rhs.channels())
        {
            return std::nullopt;
        }

        cv::Mat rt;
        cv::addWeighted(lhs, alpha, rhs, beta, 0, rt);
        return rt;
    }

    export constexpr char image_weighted_id[] = "core.image-process.weighted";
    export constexpr char image_weighted_name[] = "Weighted Addition";
    export constexpr char image_weighted_description[] = "Add two image weight";

    export dyn_node::node_definition_proxy def_image_weighted_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_weighted,
            image_weighted_id, image_weighted_name, image_weighted_description
        );
    }

}
