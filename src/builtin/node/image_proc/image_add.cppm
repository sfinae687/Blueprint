//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/3.
//

module;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

export module blueprint.builtin_node:image_add;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    export std::optional<cv::Mat> image_add(cv::Mat lhs, cv::Mat rhs)
    {
        if (lhs.channels() != rhs.channels())
        {
            return std::nullopt;
        }

        return lhs + rhs;
    }

    export constexpr char image_add_id[] = "core.image-process.add";
    export constexpr char image_add_name[] = "Add";
    export constexpr char image_add_description[] = "Add two image";

    export dyn_node::node_definition_proxy def_image_add_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_add,
            image_add_id, image_add_name, image_add_description
        );
    }

}
